/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 16:25:42 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

/* Handles input redirection (<) */
int	handle_redir_in(t_node_tree *node)
{
	int	fd;

	if (!node || !node->file)
		return (0);
	fd = open(node->file, O_RDONLY);
	if (fd < 0)
	{
		perror("minishell: open redir in");
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 redir in");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	handle_redir_out(t_node_tree *node)
{
	int	fd;

	if (!node || !node->file)
		return (0);
	fd = open(node->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("minishell: open redir out");
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 redir out");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	handle_append(t_node_tree *node)
{
	int	fd;

	if (!node || !node->file)
		return (0);
	fd = open(node->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		perror("minishell: open append");
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 append");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

/*static int	process_heredoc_pipe(int pipefd[2], const char *delimiter, \
								char **env)
{
	int	read_status;

	read_status = read_heredoc_input(pipefd[1], delimiter, env);
	close(pipefd[1]);
	if (read_status != 0)
	{
		close(pipefd[0]);
		return (1);
	}
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 heredoc");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}*/

static int	process_one_heredoc_line(char *line, int pipe_write_fd, \
									const char *delimiter, char **env, \
									bool expand)
{
	char	*line_to_write;
	int		write_status;

	if (is_heredoc_delim(line, delimiter))
		return (HEREDOC_DELIM_FOUND);
	line_to_write = line;
	if (expand)
	{
		line_to_write = expand_variables(line, env);
		if (!line_to_write)
			return (EXIT_FAILURE);
	}
	write_status = write(pipe_write_fd, line_to_write, \
							ft_strlen(line_to_write));
	if (line_to_write != line)
		free(line_to_write);
	if (write_status == -1)
	{
		perror("minishell: write heredoc pipe child");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int	heredoc_child_reader(int pipe_write_fd, const char *delimiter, \
									char **env, int input_fd_for_heredoc)
{
	char	*line;
	bool	expand_heredoc;
	int		status;

	expand_heredoc = true;
	status = EXIT_SUCCESS;
	while (status == EXIT_SUCCESS)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = get_next_line(input_fd_for_heredoc);
		if (line == NULL)
		{
			ft_putstr_fd("minishell: warning: heredoc EOF (wanted `", 2);
			ft_putstr_fd((char *)delimiter, 2);
			ft_putstr_fd("')\n", 2);
			return (EXIT_SUCCESS);
		}
		status = process_one_heredoc_line(line, pipe_write_fd, \
											delimiter, env, expand_heredoc);
		free(line);
		if (status == HEREDOC_DELIM_FOUND)
			return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

static int	handle_heredoc_parent_logic(pid_t pid, int pipefd[2])
{
	int	child_process_status;

	close(pipefd[1]);
	waitpid(pid, &child_process_status, 0);
	if (!(WIFEXITED(child_process_status) && \
		WEXITSTATUS(child_process_status) == EXIT_SUCCESS))
	{
		close(pipefd[0]);
		set_current_exit_status(WIFEXITED(child_process_status) \
			? WEXITSTATUS(child_process_status) : 1);
		return (1);
	}
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: handle_heredoc: dup2 stdin failed");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}

int	handle_heredoc(t_node_tree *node, t_shell *shell)
{
	int		pipefd[2];
	char	*delimiter;
	pid_t	pid;

	if (!shell || !shell->env || shell->saved_stdin < 0)
		return (1);
	if (!node || !node->file)
		return (ft_putstr_fd("minishell: Heredoc internal error\n", 2), 1);
	delimiter = node->file;
	if (pipe(pipefd) == -1)
		return (perror("minishell: handle_heredoc: pipe failed"), 1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (perror("minishell: handle_heredoc: fork failed"), 1);
	}
	if (pid == 0)
	{
		handle_child_signals();
		close(pipefd[0]);
		exit(heredoc_child_reader(pipefd[1], delimiter, \
									shell->env, shell->saved_stdin));
	}
	return (handle_heredoc_parent_logic(pid, pipefd));
}

