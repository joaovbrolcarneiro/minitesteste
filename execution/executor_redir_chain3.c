/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_chain3.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:05:15 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 22:54:57 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"


void	initialize_collection_data(t_redir_collection_state *state,
		t_node_tree *redir_nodes_array[])
{
	int	k;

	state->redir_nodes = redir_nodes_array;
	k = 0;
	while (k < MAX_REDIRECTIONS)
	{
		state->redir_nodes[k] = NULL;
		k++;
	}
}

int	populate_nodes_from_pattern(t_complex_pattern_params *params,
		t_redir_collection_state *state)
{
	if (params->out_r)
	{
		if (*(state->redir_count) >= MAX_REDIRECTIONS)
			return (ft_putstr_fd(ERR_MAX_REDIR, 2), 1);
		state->redir_nodes[(*(state->redir_count))++] = params->out_r;
	}
	if (params->in_r2)
	{
		if (*(state->redir_count) >= MAX_REDIRECTIONS)
			return (ft_putstr_fd(ERR_MAX_REDIR, 2), 1);
		state->redir_nodes[(*(state->redir_count))++] = params->in_r2;
	}
	if (params->in_r1)
	{
		if (*(state->redir_count) >= MAX_REDIRECTIONS)
			return (ft_putstr_fd(ERR_MAX_REDIR, 2), 1);
		state->redir_nodes[(*(state->redir_count))++] = params->in_r1;
	}
	*(state->command_node_ptr) = params->cmd_n;
	return (0);
}

int	process_complex_pattern_type1(t_complex_pattern_params *params,
		t_redir_collection_state *state)
{
	params->cmd_n = params->in_r1->left;
	if (params->in_r1->right != NULL && (params->in_r1->right->type == AST_REDIR_IN
			|| params->in_r1->right->type == AST_HEREDOC))
		params->in_r2 = params->in_r1->right;
	return (populate_nodes_from_pattern(params, state));
}

int	process_complex_pattern_type2(t_complex_pattern_params *params,
		t_redir_collection_state *state)
{
	params->in_r2 = params->in_r1->right;
	params->cmd_n = NULL;
	return (populate_nodes_from_pattern(params, state));
}

int	process_complex_pattern_type3(t_complex_pattern_params *params,
		t_redir_collection_state *state)
{
	params->in_r2 = NULL;
	params->cmd_n = NULL;
	return (populate_nodes_from_pattern(params, state));
}
