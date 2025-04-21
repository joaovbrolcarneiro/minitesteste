/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_yggdrasil.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:28:02 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 21:07:45 by jbrol-ca         ###   ########.fr       */
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

    // --- Handle Empty or Error List ---
    if (!token_lst || token_err(token_lst)) { // token_err checks pre-existing token->err flags
        // If token_err is true, an error (like unclosed quotes) was already found.
        // g_exit_code should be set already. Return NULL.
        // If token_lst is NULL (empty input), return NULL.
        return (NULL); // Nothing to parse or error already detected
    }

    // --- Find Root Token ---
    // Find the token with the highest rank (lowest precedence operator)
    // to be the root of the tree. Exclude already used tokens (shouldn't be any yet).
    root_token = token_lst;
    current = token_lst;
    while (current) {
        // Basic precedence: Higher rank means lower precedence (chosen later as root)
        // Example: PIPE (S) > CMD (B) > WORD (C)
        // If ranks are equal, left-associativity usually means picking the rightmost one.
        // Your original logic: if(root->rank <= token_lst->rank)
        if (!current->used && current->rank >= root_token->rank) { // Prioritize later tokens of same rank
             root_token = current;
        }
        current = current->next;
    }

     // If no suitable root found (e.g., all tokens used? Should not happen here)
    if (!root_token || root_token->used) {
         // This case indicates a potential issue or an unexpected token list state.
         return NULL;
    }

    // Reset g_exit_code before parsing, as make_yggdrasil might set it on syntax error
    // Keep previous exit code if needed for $? expansion *before* parsing? Complex interaction.
    // Let's assume we reset it for parsing errors. Execution will set the final code.
    // g_exit_code = 0; // Reset for parsing phase? Risky if $? needed *during* parse.

    // --- Build Tree ---
    // Start the recursive AST construction.
    // The initial segment covers the entire list [token_lst, NULL).
    // The parent node is initially NULL.
    ast_root = make_yggdrasil(root_token, token_lst, NULL, NULL);

    // --- Check for Parse Errors during AST build ---
    // If make_yggdrasil returned NULL and g_exit_code is set (e.g., to 2),
    // it means a syntax error (like missing redirect file) occurred.
    if (!ast_root && g_exit_code != 0) {
         // Error already printed by st_prsr_err, g_exit_code set.
         // Make sure any partially allocated AST in make_yggdrasil was freed.
         return (NULL); // Signal error to caller
    }

    // --- Final Check & Return ---
    // If ast_root is NULL but no error code set, it might mean the command
    // was empty after processing (e.g. only comments or empty quotes) - valid.
    return (ast_root);
}

