/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 02:03:38 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

// Note: Removed forward declaration `t_obj_ygg make_yggdrasil_init(void);`
// as it typically belongs in a header file, not a .c file.

/**
 * Checks if right child token is valid and unused.
 */
bool	has_right(t_obj_ygg obj)
{
	return (obj.right_child_token && !obj.right_child_token->used);
}

/**
 * Checks if left child token is valid and unused.
 */
bool	has_left(t_obj_ygg obj)
{
	return (obj.left_child_token && !obj.left_child_token->used);
}

/**
 * Handle parser syntax error and update exit status.
 */
void	handle_parser_error(t_token *t)
{
	st_prsr_err("syntax error near unexpected token", t->value);
	set_current_exit_status(2);
}

// This function was previously static but might need to be non-static
// depending on where mke_yggdrasil is located after splitting files.
// Assuming it needs to be non-static based on previous plan.
char	*gather_filename(t_token *redir_token, t_token *end_token)
{
	t_token	*file_token;
	char	*filename;

	if (!redir_token)
		return (NULL);
	file_token = redir_token->next;
	while (file_token && file_token != end_token && file_token->used)
		file_token = file_token->next;
	if (file_token && file_token != end_token
		&& file_token->type == TOKEN_WORD && !file_token->used)
	{
		filename = ft_strdup(file_token->value);
		file_token->used = true;
		return (filename);
	}
	return (NULL);
}
