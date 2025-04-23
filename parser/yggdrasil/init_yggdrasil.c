/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_yggdrasil.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:28:02 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/23 21:42:47 by jbrol-ca         ###   ########.fr       */
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
    t_token *root_token; // Will store the final chosen root
    t_token *last_highest_rank_token; // Stores result of original scan
    t_token *current;
    t_node_tree *ast_root = NULL;

    if (!token_lst || token_err(token_lst)) {
        return (NULL);
    }

    last_highest_rank_token = token_lst;
    current = token_lst;
    while (current) {
        // NOTE: We ignore 'used' flag here for root finding, assuming it's reset before calling
        if (current->rank >= last_highest_rank_token->rank) {
            last_highest_rank_token = current;
        }
        current = current->next;
    }

    // Initial root choice based on original logic
    root_token = last_highest_rank_token;

    // --- Step 2: Check if highest rank is Command Rank ---
    // If the highest rank found corresponds to a command, override the choice
    // to select the *first* actual command token in the sequence.
    if (root_token && root_token->rank == RANK_B) // Check if the chosen root's rank is B
    {
        current = token_lst;
        while (current) // Scan from the beginning
        {
            // Find the first token that is explicitly typed as a command
            if (current->type == TOKEN_CMD)
            {
                root_token = current; // Override: use the first command found
                break; // Stop searching once the first command is found
            }
            // Stop searching if we hit something clearly not part of the simple command
            if (current->type == TOKEN_PIPE) // Or other high-precedence operators
                break;
            current = current->next;
        }
        // If no TOKEN_CMD was found (e.g., only words like "echo"),
        // root_token remains the last_highest_rank_token (e.g., "echo").
        // The subsequent call to make_yggdrasil/gather_arguments should handle this.
    }

    // If no root found at all (e.g., empty list after checks?)
    if (!root_token) {
         return NULL;
    }

    // Reset 'used' flags before building AST
    current = token_lst;
    while (current) {
        current->used = false;
        current = current->next;
    }
    //set_current_exit_status(0); // Reset status before parsing attempt? (Careful with $?)

    // --- Step 3: Build Tree using the chosen root_token ---
    ast_root = make_yggdrasil(root_token, token_lst, NULL, NULL);

    // Error check...
    if (!ast_root && get_current_exit_status() != 0) {
        return (NULL);
    }

    return (ast_root);
}

