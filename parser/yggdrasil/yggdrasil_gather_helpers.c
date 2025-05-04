/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil_gather_helpers.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 14:45:40 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_gthr_arg	gthr_arg_vrs_init(t_gthr_arg *cu)
{
	cu->args = NULL;
	cu->temp_realloc = NULL;
	cu->i = 0;
	return (*cu);
}

static bool	gather_arg_helper3_part2(t_gthr_arg *cu)
{
	size_t	old_byte_size;
	size_t	new_byte_size;

	if (cu->i >= cu->arg_capacity)
	{
		old_byte_size = sizeof(char *) * (cu->arg_capacity + 1);
		cu->arg_capacity = cu->i + 5;
		new_byte_size = sizeof(char *) * (cu->arg_capacity + 1);
		cu->temp_realloc = ft_realloc(cu->args, old_byte_size, new_byte_size);
		if (!cu->temp_realloc)
			return (set_current_exit_status(1), true);
		cu->args = cu->temp_realloc;
	}
	return (false);
}

static bool	gather_arg_helper3_part3(t_gthr_arg *cu)
{
	if (!cu->args[cu->i])
		return (set_current_exit_status(1), true);
	cu->current->used = true;
	cu->i++;
	cu->args[cu->i] = NULL;
	return (false);
}

char	**gather_arg_helper3(t_gthr_arg *cu)
{
	int	status;

	while (cu->current)
	{
		status = process_current_token_for_args(cu);
		if (status == 1)
			break ;
		else if (status == 2)
			continue ;
		else if (status == 3)
			return (NULL);
		cu->current = cu->current->next;
	}
	return (cu->args);
}

int	process_current_token_for_args(t_gthr_arg *cu)
{
	t_token	*filename_token;

	if (cu->current->type == TOKEN_PIPE)
		return (1);
	if (cu->current->coretype == REDIR)
	{
		filename_token = cu->current->next;
		if (filename_token && filename_token->type == TOKEN_WORD)
		{
			cu->current = filename_token->next;
			return (2);
		}
		else
			return (1);
	}
	if (!cu->current->used)
	{
		if (gather_arg_helper3_part2(cu))
			return (3);
		cu->args[cu->i] = ft_strdup(cu->current->value);
		if (gather_arg_helper3_part3(cu))
			return (3);
	}
	return (0);
}
