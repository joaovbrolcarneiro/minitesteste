/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_chain2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:05:15 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 23:25:49 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	match_and_collect_complex_pattern(t_node_tree *current_node,
		t_redir_collection_state *state)
{
	t_complex_pattern_params	params;

	if (!(current_node != NULL
			&& (current_node->type == AST_REDIR_OUT
				|| current_node->type == AST_APPEND)
			&& current_node->left != NULL
			&& (current_node->left->type == AST_REDIR_IN
				|| current_node->left->type == AST_HEREDOC)))
		return (-1);
	params.out_r = current_node;
	params.in_r1 = current_node->left;
	params.in_r2 = NULL;
	params.cmd_n = NULL;
	if (params.in_r1->left != NULL && params.in_r1->left->type == AST_COMMAND)
		return (process_complex_pattern_type1(&params, state));
	else if (params.in_r1->left == NULL && params.in_r1->right != NULL
		&& (params.in_r1->right->type == AST_REDIR_IN
			|| params.in_r1->right->type == AST_HEREDOC))
		return (process_complex_pattern_type2(&params, state));
	else if (params.in_r1->left == NULL && params.in_r1->right == NULL)
		return (process_complex_pattern_type3(&params, state));
	return (-1);
}

void	advance_current_for_linear_chain(t_node_tree **current_ptr)
{
	t_node_tree	*node;

	node = *current_ptr;
	if (node->left)
		*current_ptr = node->left;
	else if (node->right && (node->right->type >= AST_REDIR_IN
			&& node->right->type <= AST_HEREDOC))
		*current_ptr = node->right;
	else if (node->right && node->right->type == AST_COMMAND)
		*current_ptr = node->right;
	else
		*current_ptr = NULL;
}

int	process_node_in_linear_chain(t_node_tree **current_ptr,
		t_redir_collection_state *state)
{
	t_node_tree	*node;

	node = *current_ptr;
	if (node->type >= AST_REDIR_IN && node->type <= AST_HEREDOC)
	{
		if (*(state->redir_count) >= MAX_REDIRECTIONS)
			return (ft_putstr_fd(ERR_MAX_REDIR, 2), 1);
		state->redir_nodes[(*(state->redir_count))++] = node;
		advance_current_for_linear_chain(current_ptr);
	}
	else if (node->type == AST_COMMAND)
	{
		*(state->command_node_ptr) = node;
		*current_ptr = NULL;
	}
	else if (node->type == AST_REDIR_OUT || node->type == AST_APPEND)
	{
		if (*(state->redir_count) >= MAX_REDIRECTIONS)
			return (ft_putstr_fd(ERR_MAX_REDIR, 2), 1);
		state->redir_nodes[(*(state->redir_count))++] = node;
		*current_ptr = node->left;
	}
	else
		*current_ptr = NULL;
	return (0);
}

int	collect_linear_chain_nodes(t_node_tree *current_ast_node,
		t_redir_collection_state *state)
{
	t_node_tree	*current;
	int			status;

	current = current_ast_node;
	status = 0;
	while (current && status == 0)
	{
		status = process_node_in_linear_chain(&current, state);
	}
	return (status);
}

/* --- Static Main Collector Function --- */
static int	collect_redir_and_command_nodes(
	t_node_tree *redir_nodes_array[],
	int *redir_count_val,
	t_node_tree **command_node_val,
	t_node_tree *current_ast_node)
{
	t_redir_collection_state	state;
	int							pattern_res;

	state.redir_count = redir_count_val;
	state.command_node_ptr = command_node_val;
	initialize_collection_data(&state, redir_nodes_array);
	pattern_res = match_and_collect_complex_pattern(current_ast_node, &state);
	if (pattern_res == 0)
		return (0);
	else if (pattern_res == 1)
		return (1);
	return (collect_linear_chain_nodes(current_ast_node, &state));
}
