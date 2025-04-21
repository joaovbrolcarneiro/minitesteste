/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_node0.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:52:43 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 21:27:02 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_token *new_token_super(t_token *token)
{
	return (token);
}


t_token *remove_node_after(t_token *prev_node) {
    t_token *node_to_remove;

    if (!prev_node || !prev_node->next) {
        return NULL;
    }
    node_to_remove = prev_node->next;
    prev_node->next = node_to_remove->next; // Bypass the node
    node_to_remove->next = NULL; // Isolate the removed node
    return node_to_remove;
}

// Frees a single token and its allocated value
void free_single_token(t_token *token) {
    if (!token) return;
    if (token->value) {
        free(token->value);
    }
    // Free args/file if they were ever allocated for this token type
    // if (token->args) ft_free_strarray(token->args);
    // if (token->file) free(token->file);
    free(token);
}


// New function to perform concatenation after quote handling
void perform_quote_concatenation(t_token *token_list) {
    t_token *current = token_list;
    t_token *node_to_remove;
    char *joined_value;
    char *temp_value_ptr; // To hold old pointer during join

    while (current) {
        // Check if current node should be joined with the next one
        if (current->join_next && current->next) {
            // Attempt to join current->value and current->next->value
            temp_value_ptr = current->value; // Store old pointer
            joined_value = ft_strjoin(current->value, current->next->value);

            if (!joined_value) {
                perror("konosubash: perform_quote_concatenation: ft_strjoin failed");
                // Decide how to handle join failure. Maybe set an error flag?
                // For now, we'll stop trying to join this token.
                current->join_next = false; // Prevent further attempts on this token
                current = current->next; // Move to next token
                continue;
            }

            // Free the old value of the current token BEFORE assigning the new one
            free(temp_value_ptr);
            current->value = joined_value; // Update current token's value

            // The current token should inherit the join_next status of the removed token
            current->join_next = current->next->join_next;

            // Remove the next node from the list
            node_to_remove = remove_node_after(current); // Removes current->next

            if (node_to_remove) {
                // Free the removed token structure and its original value
                free_single_token(node_to_remove);
            }

            // --- IMPORTANT ---
            // Stay on the *current* node. Do not advance `current = current->next;` here.
            // This allows for multiple concatenations like "a""b""c".
            // The loop condition will eventually advance `current` when `current->join_next` is false
            // or when `current->next` is NULL.

        } else {
            // No join needed for this token, move to the next one
            current = current->next;
        }
    }
}



t_token	*handler_args_file(t_token *token, t_token *first)
{
	t_token	*current;
	t_token	*next;

	current = token;
	while (current)
	{
		next = current->next;
		if (current->coretype == REDIR)
		{
			redir_handler_file(current, first);
		}
		else
		{
			cmd_handler_args(current, first);
		}
		current = next;
	}
	return (first);
}

t_token	*redir_handler_file(t_token *token, t_token *first)
{
	t_token	*file_node;

	if (!token || token->coretype != REDIR || !token->next
		|| token->next->rank == RANK_S)
		return (token);
	file_node = rm_node_lst(token->next, first);
	if (file_node)
	{
		token->file = file_node->value;
	}
	else
	{
		token->file = NULL;
	}
	return (token);
}