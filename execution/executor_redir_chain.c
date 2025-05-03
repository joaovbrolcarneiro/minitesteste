/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_chain.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 16:51:30 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

static int	process_redir_chain_node(t_node_tree **current, \
	t_node_tree *redir_nodes[], int *redir_count, t_node_tree **command_node)
{
	if ((*current)->type >= AST_REDIR_IN && (*current)->type <= AST_HEREDOC)
	{
		if (*redir_count < MAX_REDIRECTIONS)
			redir_nodes[(*redir_count)++] = *current;
		else
			return (ft_putstr_fd("konosubash: too many redirections\n", 2), 1);
		if ((*current)->left)
			*current = (*current)->left;
		else if ((*current)->right)
			*current = (*current)->right;
		else
			*current = NULL;
	}
	else if ((*current)->type == AST_COMMAND)
	{
		*command_node = *current;
		*current = NULL;
	}
	else
	{
		ft_putstr_fd("konosubash: internal error: unexpected node type\n", 2);
		return (3);
	}
	return (0);
}

/* Assuming collect_redir_and_command_nodes uses the above correctly */
static int	collect_redir_and_command_nodes(t_node_tree *redir_nodes[], \
	int *redir_count, t_node_tree **command_node, t_node_tree *node)
{
	t_node_tree	*current;
	int			status;

	current = node;
	while (current)
	{
		status = process_redir_chain_node(&current, redir_nodes, \
						redir_count, command_node);
		if (status != 0)
			return (status);
		if (*command_node)
			break ;
	}
	return (0);
}

/* Assuming apply_redirection_nodes uses handle_redirections correctly */
static int	apply_redirection_nodes(t_node_tree *redir_nodes[], int redir_count)
{
	int	i;
	int	status;

	i = redir_count - 1;
	status = 0;
	while (i >= 0)
	{
		status = handle_redirections(redir_nodes[i]);
		if (status != 0)
			return (status);
		i--;
	}
	return (0);
}

/* Modified execute_redir_chain_core */
static int	execute_redir_chain_core(t_shell *shell, t_node_tree *node)
{
	int			status;
	t_node_tree	*command_node;
	t_node_tree	*redir_nodes[MAX_REDIRECTIONS];
	int			redir_count;

	command_node = NULL;
	redir_count = 0;
	status = collect_redir_and_command_nodes(redir_nodes, &redir_count, \
											&command_node, node);
	if (status != 0)
		return (set_current_exit_status(status), status);
	status = 0;
	if (redir_count > 0)
		status = apply_redirection_nodes(redir_nodes, redir_count);
	if (status == 0)
	{
		if (command_node)
			status = execute_simple_command(shell, command_node);
		else
			status = 0;
	}
	set_current_exit_status(status);
	return (status);
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
