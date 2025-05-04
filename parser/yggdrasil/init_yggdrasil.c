/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_yggdrasil.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:28:02 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 02:01:39 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

bool	token_err(t_token *token_lst)
{
	while (token_lst)
	{
		if (token_lst->err == 1)
			return (true);
		token_lst = token_lst->next;
	}
	return (false);
}

static t_token	*find_initial_root(t_token *token_lst)
{
	t_token	*last_highest_rank_token;
	t_token	*current;

	if (!token_lst)
		return (NULL);
	last_highest_rank_token = token_lst;
	current = token_lst;
	while (current)
	{
		if (current->rank >= last_highest_rank_token->rank)
		{
			last_highest_rank_token = current;
		}
		current = current->next;
	}
	return (last_highest_rank_token);
}

static t_token	*find_first_command_override(t_token *token_lst,
		t_token *initial_root)
{
	t_token	*current;

	if (initial_root && initial_root->rank == RANK_B)
	{
		current = token_lst;
		while (current)
		{
			if (current->type == TOKEN_CMD)
				return (current);
			if (current->type == TOKEN_PIPE)
				break ;
			current = current->next;
		}
	}
	return (initial_root);
}

static void	reset_token_used_flags(t_token *token_lst)
{
	t_token	*current;

	current = token_lst;
	while (current)
	{
		current->used = false;
		current = current->next;
	}
}

t_node_tree	*init_yggdrasil(t_token *token_lst)
{
	t_token		*root_token;
	t_node_tree	*ast_root;

	ast_root = NULL;
	if (!token_lst || token_err(token_lst))
		return (NULL);
	root_token = find_initial_root(token_lst);
	root_token = find_first_command_override(token_lst, root_token);
	if (!root_token)
		return (NULL);
	reset_token_used_flags(token_lst);
	ast_root = mke_yggdrasil(root_token, token_lst, NULL, NULL);
	if (!ast_root && get_current_exit_status() != 0)
		return (NULL);
	return (ast_root);
}
