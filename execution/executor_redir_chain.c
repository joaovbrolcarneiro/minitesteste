/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_chain.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 22:56:18 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	execute_redir_chain_core(t_shell *shell, t_node_tree *node)
{
	int			status;
	t_node_tree	*command_node;
	t_node_tree	*redir_nodes[MAX_REDIRECTIONS];
	int			redir_count;

	command_node = NULL;
	redir_count = 0;
	status = collect_redir_and_command_nodes(redir_nodes, &redir_count,
			&command_node, node);
	if (status != 0)
	{
		set_current_exit_status(status);
		return (status);
	}
	status = 0;
	if (redir_count > 0)
		status = apply_redirection_nodes(redir_nodes, redir_count, shell);
	if (status == 0)
	{
		if (command_node)
			status = execute_simple_command(shell, command_node);
		else if (redir_count > 0)
			status = 0;
	}
	return (set_current_exit_status(status), status);
}


/* Assuming apply_redirection_nodes uses handle_redirections correctly */
int	process_individual_redirections(t_node_tree *redir_nodes[],
											int redir_count, t_shell *shell,
											int *final_output)
{
	int	i;
	int	status;

	status = 0;
	i = redir_count - 1;
	while (i >= 0)
	{
		if (redir_nodes[i] == NULL && (i--, 1))
			continue;
		if (redir_nodes[i]->type == AST_HEREDOC)
			status = handle_heredoc(redir_nodes[i], shell);
		else if (redir_nodes[i]->type == AST_REDIR_IN)
			status = handle_redir_in(redir_nodes[i]);
		else if (redir_nodes[i]->type == AST_REDIR_OUT
			|| redir_nodes[i]->type == AST_APPEND)
			status = open_and_set_final_output_fd(redir_nodes[i],
					final_output);
		if (status != 0)
			return ((*final_output != -1 && close(*final_output)), status);
		i--;
	}
	return (0);
}

/*
 * Applies all redirections for a simple command or redirection sequence.
 * Manages the overall process of applying redirections, including setting
 * the final STDOUT.
 */
int	apply_redirection_nodes(t_node_tree *redir_nodes[],
									int redir_count, t_shell *shell)
{
	int	status;
	int	final_output_fd;

	final_output_fd = -1;
	status = process_individual_redirections(redir_nodes, redir_count,
			shell, &final_output_fd);
	if (status != 0)
		return (status);
	if (final_output_fd != -1)
	{
		if (dup2(final_output_fd, STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2 final_output_fd to STDOUT_FILENO");
			close(final_output_fd);
			return (1);
		}
		close(final_output_fd);
	}
	return (0);
}

int	execute_redirection_chain(t_shell *shell, t_node_tree *node)
{
	int	status;
	int	original_fds[2];
	int	fds_saved;

	if (!node)
		return (1);
	fds_saved = (save_original_fds(original_fds) == 0);
	status = 0;
	if (!fds_saved)
	{
		perror("konosubash: execute_redirection_chain: Failed to save fds");
		status = 1;
		set_current_exit_status(status);
		return (status);
	}
	status = execute_redir_chain_core(shell, node);
	restore_fds(original_fds);
	close_fds(original_fds);
	return (status);
}
