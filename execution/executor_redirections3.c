/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections3.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 23:47:31 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	wait_for_heredoc_child(pid_t pid, int *child_status_ptr)
{
	int	wait_ret;

	wait_ret = waitpid(pid, child_status_ptr, 0);
	while (wait_ret == -1 && errno == EINTR)
	{
		wait_ret = waitpid(pid, child_status_ptr, 0);
	}
	if (wait_ret == -1)
	{
		perror("minishell: waitpid failed in heredoc_parent_logic");
		set_current_exit_status(1);
		return (1);
	}
	return (0);
}

/*
 * Processes the exit status of the heredoc child if it exited normally.
 * Returns 0 if child exited successfully, 1 otherwise.
 * Closes pipe_read_fd on error.
 */
int	process_heredoc_child_exit_status(int child_status, int pipe_read_fd)
{
	if (WEXITSTATUS(child_status) != EXIT_SUCCESS)
	{
		close(pipe_read_fd);
		set_current_exit_status(WEXITSTATUS(child_status));
		return (1);
	}
	return (0);
}

/*
 * Processes the status of the heredoc child if it was terminated by a signal.
 * Sets appropriate exit status based on the signal.
 * Always returns 1 (failure for heredoc processing). Closes pipe_read_fd.
 */
int	process_heredoc_child_signal_status(int child_status, int pipe_read_fd)
{
	close(pipe_read_fd);
	if (WTERMSIG(child_status) == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		set_current_exit_status(130);
	}
	else
	{
		set_current_exit_status(128 + WTERMSIG(child_status));
	}
	return (1);
}

/*
 * Parent process logic after forking for heredoc.
 * Waits for child, processes its status, and sets up STDIN from pipe.
 */
int	handle_heredoc_parent_logic(pid_t pid, int pipefd[2])
{
	int	child_process_status;
	int	status_check_result;

	close(pipefd[1]);
	if (wait_for_heredoc_child(pid, &child_process_status) != 0)
		return (close(pipefd[0]), 1);
	if (WIFEXITED(child_process_status))
	{
		status_check_result = process_heredoc_child_exit_status(
				child_process_status, pipefd[0]);
		if (status_check_result != 0)
			return (status_check_result);
	}
	else if (WIFSIGNALED(child_process_status))
	{
		return (process_heredoc_child_signal_status(
				child_process_status, pipefd[0]));
	}
	else
		return (close(pipefd[0]), set_current_exit_status(1), 1);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		return (1, close(pipefd[0]));
	return (close(pipefd[0]), 0);
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
		perror("konosubash: handle_heredoc: pipe failed");
		return (1);
	}
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		perror("konosubash: handle_heredoc: fork failed");
		return (1);
	}
	if (pid == 0)
	{
		execute_heredoc_child(pipefd[1], pipefd[0], delimiter, shell);
	}
	return (handle_heredoc_parent_logic(pid, pipefd));
}
