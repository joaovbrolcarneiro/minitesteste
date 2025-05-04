/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil_finders.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 02:13:49 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_token	*find_right_token(t_token *token, t_token *eof)
{
	t_token	*father;
	int		i;

	i = 0;
	if (!token)
		return (NULL);
	if (token->next == eof || !token->next || token == eof)
		return (NULL);
	father = token;
	token = token->next;
	while (1)
	{
		if (father->rank - i == token->rank)
			return (token);
		if (father->rank - i == RANK_F)
			return (NULL);
		token = token->next;
		if (token == eof)
		{
			i++;
			token = father->next;
		}
	}
	return (token);
}

t_token	*find_left_token(t_token *target_node, t_token *first_in_segment)
{
	t_flt_vrs	vars;

	if (flt_nrm(target_node, first_in_segment, &vars))
		return (NULL);
	while (vars.cu_tken != NULL)
	{
		if (!vars.cu_tken->used && vars.cu_tken->rank >= vars.max_rank_found)
			flt_nrm2(&vars);
		if (vars.cu_tken == first_in_segment)
			break ;
		vars.cu_tken = get_prev_node(vars.cu_tken, first_in_segment);
	}
	if (!vars.last_highest_found)
		return (NULL);
	if (vars.last_highest_found->rank == RANK_B)
	{
		vars.cu_tken = first_in_segment;
		while (vars.cu_tken && vars.cu_tken != target_node)
		{
			if (vars.cu_tken->type == TOKEN_CMD && !vars.cu_tken->used)
				return (vars.cu_tken);
			vars.cu_tken = vars.cu_tken->next;
		}
	}
	return (vars.last_highest_found);
}

bool	flt_nrm(t_token *target_node, t_token *first_in_segment, t_flt_vrs *vrs)
{
	if (target_node == first_in_segment || !target_node || !first_in_segment)
		return (true);
	vrs->last_highest_found = NULL;
	vrs->max_rank_found = RANK_F;
	vrs->cu_tken = get_prev_node(target_node, first_in_segment);
	return (false);
}

void	flt_nrm2(t_flt_vrs *vrs)
{
	vrs->max_rank_found = vrs->cu_tken->rank;
	vrs->last_highest_found = vrs->cu_tken;
}
