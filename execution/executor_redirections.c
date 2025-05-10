/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 21:05:10 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	handle_heredoc_parent_logic(pid_t pid, int pipefd[2])
{
	int	child_process_status;

	close(pipefd[1]);
	waitpid(pid, &child_process_status, 0);
	if (!(WIFEXITED(child_process_status)
			&& WEXITSTATUS(child_process_status) == EXIT_SUCCESS))
	{
		close(pipefd[0]);
		if (WIFEXITED(child_process_status))
			set_current_exit_status(WEXITSTATUS(child_process_status));
		else
			set_current_exit_status(1);
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

	delimiter = heredoc_init_and_get_delimiter(node, shell);
	if (!delimiter)
		return (1);
	if (pipe(pipefd) == -1)
	{
		perror("minishell: handle_heredoc: pipe failed");
		return (1);
	}
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		perror("minishell: handle_heredoc: fork failed");
		return (1);
	}
	if (pid == 0)
		execute_heredoc_child(pipefd[1], pipefd[0], delimiter, shell);
	return (handle_heredoc_parent_logic(pid, pipefd));
}

int	handle_heredoc_eof(const char *delimiter)
{
	ft_putstr_fd("minishell: warning: heredoc EOF (wanted `", 2);
	ft_putstr_fd((char *)delimiter, 2);
	ft_putstr_fd("')\n", 2);
	return (EXIT_SUCCESS);
}

char	*heredoc_init_and_get_delimiter(t_node_tree *node, t_shell *shell)
{
	if (!shell || !shell->env || shell->saved_stdin < 0)
		return (NULL);
	if (!node || !node->file)
	{
		ft_putstr_fd("minishell: Heredoc internal error\n", 2);
		return (NULL);
	}
	return (node->file);
}

// Helper 2: Child process execution logic
// Assuming handle_child_signals(), exit() are defined/available
void	execute_heredoc_child(int pipe_write_fd, int pipe_read_fd,
								const char *delimiter, t_shell *shell)
{
	handle_child_signals();
	close(pipe_read_fd);
	exit(heredoc_child_reader(pipe_write_fd, delimiter,
			shell->env, shell->saved_stdin));
}