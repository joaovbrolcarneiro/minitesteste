/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil_gather.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 02:14:17 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

static int	count_following_words(t_token *cmd_token)
{
	t_token	*current;
	int		count;

	count = 0;
	if (!cmd_token)
		return (0);
	current = cmd_token->next;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
			break ;
		if (current->coretype == REDIR)
		{
			if (current->next && current->next->type == TOKEN_WORD)
			{
				current = current->next->next;
				continue ;
			}
			else
				break ;
		}
		count++;
		current = current->next;
	}
	return (count);
}

char	**gather_arguments(t_token *cmd_token, t_token *segment_end_token)
{
	t_gthr_arg_vrs	cu;

	(void)segment_end_token;
	cu = gthr_arg_vrs_init(&cu);
	if (gather_arg_helper(cmd_token, &cu))
		return (NULL);
	if (gather_arg_helper2(cmd_token, &cu))
		return (NULL);
	cu.current = cmd_token->next;
	cu.args = gather_arg_helper3(&cu);
	return (cu.args);
}

bool	gather_arg_helper(t_token *cmd_token, t_gthr_arg_vrs *cu)
{
	if (!cmd_token || !cmd_token->value)
		return (true);
	cu->arg_count_total = count_following_words(cmd_token);
	cu->arg_capacity = cu->arg_count_total + 1;
	cu->args = hb_malloc(sizeof(char *) * (cu->arg_capacity + 1));
	return (false);
}

bool	gather_arg_helper2(t_token *cmd_token, t_gthr_arg_vrs *cu)
{
	if (!cu->args)
		return (set_current_exit_status(1), true);
	cu->args[0] = cmd_token->value;
	cu->i = 1;
	cu->args[cu->i] = NULL;
	return (false);
}
