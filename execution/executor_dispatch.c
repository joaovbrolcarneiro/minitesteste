/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_dispatch.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 20:05:21 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Part 2 of handle_redir_execution: apply redir and exec command
*/
int	execute_redir_after_save(t_shell *shell, t_node_tree *node)
{
	int	status;
	int	internal_status;

	status = 0;
	internal_status = handle_redirections(node, shell); // Pass shell
	if (internal_status != 0)
		status = internal_status; // Use actual status from redirection
	else
		status = execute_redir_cmd_node(shell, node);
	return (status);
}

/*
** Dispatches execution for redirection nodes
*/
int	handle_redir_execution(t_shell *shell, t_node_tree *node)
{
	int	status;
	int	original_fds[2];
	int	fds_saved;

	status = 0;
	fds_saved = 0;
	if (save_original_fds(original_fds) == 0)
		fds_saved = 1;
	else
		status = 1;
	if (status == 0)
		status = execute_redir_after_save(shell, node);
	if (fds_saved)
	{
		restore_fds(original_fds);
		close_fds(original_fds);
	}
	return (status);
}

/*
** Part 2 of handle_command_execution: execute command
*/
int	execute_command_after_save(t_shell *shell, t_node_tree *node)
{
	int	status;

	status = execute_simple_command(shell, node);
	return (status);
}

/*
** Dispatches execution for simple command nodes
*/
int	handle_command_execution(t_shell *shell, t_node_tree *node)
{
	int	status;
	int	original_fds[2];
	int	fds_saved;

	status = 0;
	fds_saved = 0;
	if (save_original_fds(original_fds) == 0)
		fds_saved = 1;
	else
		status = 1;
	if (status == 0)
		status = execute_command_after_save(shell, node);
	if (fds_saved)
	{
		restore_fds(original_fds);
		close_fds(original_fds);
	}
	return (status);
}

/*
** Dispatches execution for PIPE nodes
*/
int	handle_pipe_execution(t_shell *shell, t_node_tree *node)
{
	return (execute_pipe_command(shell, node));
}

// Note: execute_redirection_chain and its helpers have been moved
// to executor_redir_chain.c