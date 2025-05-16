/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections4.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 19:53:34 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	handle_heredoc_eof(const char *delimiter)
{
	ft_putstr_fd("konosubash: warning: heredoc EOF (wanted `", 2);
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

void	handle_heredoc_dup_error(const char *message, int bkp_in,
		int bkp_out, int pipe_wfd)
{
	perror(message);
	if (bkp_in != -1)
		close(bkp_in);
	if (bkp_out != -1)
		close(bkp_out);
	close(pipe_wfd);
	exit(EXIT_FAILURE);
}

/*
 * Prepares file descriptors for the heredoc child process.
 * - Backs up current STDIN and STDOUT.
 * - Validates shell's saved_stdin and saved_stdout.
 * - Redirects STDIN to shell's saved_stdin.
 * - Redirects STDOUT to shell's saved_stdout.
 * Exits fatally if any operation fails.
 */
void	prepare_child_fds_for_heredoc(t_shell *shell, int *bkp_in_ptr,
		int *bkp_out_ptr, int pipe_wfd)
{
	*bkp_in_ptr = dup(STDIN_FILENO);
	*bkp_out_ptr = dup(STDOUT_FILENO);
	if (shell->saved_stdin == -1 || shell->saved_stdout == -1)
	{
		ft_putstr_fd("minishell: critical: saved_fds invalid in child.\n",
			STDERR_FILENO);
		if (*bkp_in_ptr != -1)
			close(*bkp_in_ptr);
		if (*bkp_out_ptr != -1)
			close(*bkp_out_ptr);
		close(pipe_wfd);
		exit(EXIT_FAILURE);
	}
	if (dup2(shell->saved_stdin, STDIN_FILENO) == -1)
		handle_heredoc_dup_error("minishell: heredoc_child dup2 saved_stdin",
			*bkp_in_ptr, *bkp_out_ptr, pipe_wfd);
	if (dup2(shell->saved_stdout, STDOUT_FILENO) == -1)
		handle_heredoc_dup_error("minishell: heredoc_child dup2 saved_stdout",
			*bkp_in_ptr, *bkp_out_ptr, pipe_wfd);
}

void	heredoc_sigint_empty_handler(int signum)
{
	(void)signum;
}
