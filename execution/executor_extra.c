/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_extra.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:55:26 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	handle_assignment_execution(t_node_tree *node)
{
	(void)node;
	return (0);
}

void	handle_exit_status(t_exp_vars *v)
{
	char	*exit_str;

	exit_str = ft_itoa(get_current_exit_status());
	if (!exit_str)
	{
		perror("minishell: ft_itoa");
		return ;
	}
	append_str_to_exp_vars(v, exit_str);
	v->i++;
}

bool	proximity_exception(char *input, int i)
{
	if ((!ischarset("|<>", input[i]) && input[i] != ' ') && \
	input[i] && !ischarset("|<>", *input))
		return (true);
	return (false);
}

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' \
	|| c == '\v' || c == '\f' || c == '\r');
}

/* Cleans up shell resources */
void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env)
	{
		shell->env = NULL;
	}
	if (shell->saved_stdin >= 0)
	{
		close(shell->saved_stdin);
		shell->saved_stdin = -1;
	}
	if (shell->saved_stdout >= 0)
	{
		close(shell->saved_stdout);
		shell->saved_stdout = -1;
	}
	get_next_line(GNL_CLEANUP);
	minigarbege_colector();
}
