/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 21:04:57 by jbrol-ca         ###   ########.fr       */
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

int	process_one_heredoc_line(char *line,
									t_heredoc_line_params *params)
{
	char	*line_to_write;
	int		write_status;

	if (is_heredoc_delim(line, params->delimiter))
		return (HEREDOC_DELIM_FOUND);
	line_to_write = line;
	if (params->expand)
	{
		line_to_write = expand_variables(line, params->env);
		if (!line_to_write)
			return (EXIT_FAILURE);
	}
	write_status = write(params->pipe_write_fd, line_to_write,
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

int	heredoc_child_reader(int pipe_write_fd, const char *delimiter,
								char **env, int input_fd_for_heredoc)
{
	char					*line;
	int						status;
	t_heredoc_line_params	line_params;

	status = EXIT_SUCCESS;
	line_params.pipe_write_fd = pipe_write_fd;
	line_params.delimiter = delimiter;
	line_params.env = env;
	line_params.expand = true;
	while (status == EXIT_SUCCESS)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = get_next_line(input_fd_for_heredoc);
		if (line == NULL)
			return (handle_heredoc_eof(delimiter));
		status = process_one_heredoc_line(line, &line_params);
		free(line);
		if (status == HEREDOC_DELIM_FOUND)
			return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}
