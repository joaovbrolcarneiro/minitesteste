/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 19:41:46 by jbrol-ca         ###   ########.fr       */
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

// This function was previously static but might need to be non-static
// depending on where mke_yggdrasil is located after splitting files.
// Assuming it needs to be non-static based on previous plan.
static char	*try_strdup_filename(t_token *file_token)
{
	char	*filename;
	bool	is_valid_type;
	bool	is_not_operator;

	is_valid_type = (file_token->type == TOKEN_WORD
			|| file_token->type == TOKEN_CMD);
	is_not_operator = (file_token->coretype != TOKEN_PIPE
			&& file_token->coretype != REDIR);
	if (is_valid_type && is_not_operator)
	{
		filename = ft_strdup(file_token->value);
		if (!filename)
		{
			perror("minishell: gather_filename: strdup failed");
			set_current_exit_status(1);
			return (NULL);
		}
		file_token->used = true;
		return (filename);
	}
	return (NULL);
}

char	*gather_filename(t_token *redir_token, t_token *end_token)
{
	t_token	*file_token;
	char	*filename;

	filename = NULL;
	if (!redir_token)
		return (NULL);
	file_token = redir_token->next;
	while (file_token && file_token != end_token && file_token->used)
		file_token = file_token->next;
	if (file_token && file_token != end_token && !file_token->used)
		filename = try_strdup_filename(file_token);
	return (filename);
}
