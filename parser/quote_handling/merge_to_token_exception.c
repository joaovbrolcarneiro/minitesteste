/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   merge_to_token_exception.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 19:17:54 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 21:21:21 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"


bool merge_to_token_exception(t_token *token)
{
    // Function disabled to prevent non-standard token merging.
    (void)token; // Mark token as unused
    return (false);
}

/*bool merge_to_token_exception(t_token *token) - funcao antiga
{
	t_token *first;
	t_token *rm_node;
	first = token;
	if(token->merge_next == true)
	{
		rm_node = rm_node_lst(token->next, first);
		token->value = ft_strjoin(token->value, rm_node->value);
	}
	return (true);
}*/