/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_node0.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:52:43 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 17:21:58 by hde-barr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_token	*new_token_super(t_token *token)
{
	return (token);
}

t_token	*remove_node_after(t_token *prev_node)
{
	t_token	*node_to_remove;

	if (!prev_node || !prev_node->next)
	{
		return (NULL);
	}
	node_to_remove = prev_node->next;
	prev_node->next = node_to_remove->next;
	node_to_remove->next = NULL;
	return (node_to_remove);
}

/*void free_single_token(t_token *token) {
    if (!token) 
		return;
    if (token->value) 
	{
        free(token->value); //free?
    }
    free(token); // free?
}*/

void	perform_quote_con_part2(t_token	*current, t_token	*node_to_remove, \
char	*joined_value, char	*temp_value_ptr)
{
	while (current)
	{
		if (current->join_next && current->next
			&& current->coretype != TOKEN_PIPE
			&& current->coretype != REDIR
			&& current->next->coretype != TOKEN_PIPE
			&& current->next->coretype != REDIR)
		{
			temp_value_ptr = current->value;
			joined_value = ft_strjoin(current->value, current->next->value);
			if (!joined_value)
			{
				perror("konosubash: pqc: strjoin failed");
				current->join_next = false;
				current = current->next;
				continue ;
			}
			current->value = joined_value;
			current->join_next = current->next->join_next;
			node_to_remove = remove_node_after(current);
		}
		else
			current = current->next;
	}
}

void	perform_quote_concatenation(t_token *token_list)
{
	t_token	*currnt;
	t_token	*node_to_rm;
	char	*joined_value;
	char	*temp_value_ptr;

	node_to_rm = NULL;
	joined_value = NULL;
	temp_value_ptr = NULL;
	currnt = token_list;
	perform_quote_con_part2(currnt, node_to_rm, joined_value, temp_value_ptr);
}

/* perform_quote_concatenation old!!!!!
void	perform_quote_concatenation(t_token *token_list)
{
	t_token	*current;
	t_token	*node_to_remove;
	char	*joined_value;
	char	*temp_value_ptr;

	current = token_list;
	while (current)
	{
		if (current->join_next && current->next
			&& current->coretype != TOKEN_PIPE
			&& current->coretype != REDIR
			&& current->next->coretype != TOKEN_PIPE
			&& current->next->coretype != REDIR)
		{
			temp_value_ptr = current->value;
			joined_value = ft_strjoin(current->value, current->next->value);
			if (!joined_value)
			{
				perror("konosubash: pqc: strjoin failed");
				current->join_next = false;
				current = current->next;
				continue ;
			}
			free(temp_value_ptr); // free?
			current->value = joined_value;
			current->join_next = current->next->join_next;
			node_to_remove = remove_node_after(current);
			if (node_to_remove)
				free_single_token(node_to_remove);
		}
		else
			current = current->next;
	}
}
*/

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
