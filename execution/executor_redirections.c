/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 19:38:22 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

/*
 * Finalizes the heredoc child process.
 * - Restores original STDIN and STDOUT from backups (best effort).
 * - Closes backup file descriptors.
 * - Performs GNL cleanup and custom garbage collection.
 * - Closes the write end of the pipe.
 * - Exits with the provided exit code.
 */
void	finalize_heredoc_child(int bkp_in, int bkp_out,
		int pipe_wfd, int exit_code)
{
	if (bkp_in != -1)
		dup2(bkp_in, STDIN_FILENO);
	if (bkp_out != -1)
		dup2(bkp_out, STDOUT_FILENO);
	if (bkp_in != -1)
		close(bkp_in);
	if (bkp_out != -1)
		close(bkp_out);
	get_next_line(GNL_CLEANUP);
	minigarbege_colector();
	close(pipe_wfd);
	exit(exit_code);
}

/*
 * Main execution logic for the heredoc child process.
 * Sets up signals, prepares file descriptors, reads heredoc input,
 * and then finalizes and exits.
 */
void	execute_heredoc_child(int pipe_write_fd, int pipe_read_fd,
		const char *delimiter, t_shell *shell)
{
	int	child_stdin_backup;
	int	child_stdout_backup;
	int	reader_status;

	handle_child_signals();
	signal(SIGINT, heredoc_sigint_empty_handler);
	close(pipe_read_fd);
	prepare_child_fds_for_heredoc(shell, &child_stdin_backup,
		&child_stdout_backup, pipe_write_fd);
	reader_status = heredoc_child_reader(pipe_write_fd, delimiter,
			shell->env, STDIN_FILENO);
	if (reader_status == HEREDOC_INTERRUPTED)
	{
		finalize_heredoc_child(child_stdin_backup, child_stdout_backup,
			pipe_write_fd, 130);
	}
	else
	{
		finalize_heredoc_child(child_stdin_backup, child_stdout_backup,
			pipe_write_fd, reader_status);
	}
}

int	open_and_set_final_output_fd(t_node_tree *node, int *current_out_fd)
{
	int	flags;
	int	new_fd;

	if (*current_out_fd != -1)
	{
		close(*current_out_fd);
		*current_out_fd = -1;
	}
	flags = O_WRONLY | O_CREAT;
	if (node->type == AST_REDIR_OUT)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	new_fd = open(node->file, flags, 0644);
	if (new_fd < 0)
	{
		ft_putstr_fd("konosubash: ", STDERR_FILENO);
		ft_putstr_fd(node->file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		perror(NULL);
		return (1);
	}
	*current_out_fd = new_fd;
	return (0);
}
