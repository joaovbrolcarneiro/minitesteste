/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_yggdrasil.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:28:02 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 22:44:32 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

bool token_err(t_token *token_lst)
{
	while (token_lst)
	{
		//printf("%d\n",token_lst->err);
		if(token_lst->err == 1)
			return (true);
		token_lst = token_lst->next;
	}
	return (false);
}

t_node_tree *init_yggdrasil(t_token *token_lst)
{
    t_token *root_token;
    t_token *current;
    t_node_tree *ast_root = NULL;

    if (!token_lst || token_err(token_lst)) 
    { 

        return (NULL);
    }

    root_token = token_lst;
    current = token_lst;
    while (current) {
        if (!current->used && current->rank >= root_token->rank) 
        {
             root_token = current;
        }
        current = current->next;
    }

    if (!root_token || root_token->used) 
    {
         return NULL;
    }


    ast_root = make_yggdrasil(root_token, token_lst, NULL, NULL);

    if (!ast_root && g_exit_code != 0) 
    {
         return (NULL);
    }

    return (ast_root);
}

