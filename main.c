/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 23:18:56 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

static int	g_shell_exit_status;

void	quote_handler_call_loop(t_token *token, char **env)
{
	int	is_unclosed;

	while (token)
	{
		is_unclosed = 0;
		token->value = quote_handler(token, env, &is_unclosed);
		if (is_unclosed == 1)
			token->err = 1;
		else
			token->err = 0;
		token = token->next;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	set_current_exit_status(0);
	init_shell(&shell, envp);
	readline_loop(&shell);
	get_next_line(GNL_CLEANUP);
	cleanup_shell(&shell);
	return (get_current_exit_status());
}

bool	is_builtin(const char *cmd)
{
	if (!cmd)
		return (false);
	if (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0)
	{
		return (true);
	}
	return (false);
}

void	set_current_exit_status(int status)
{
	g_shell_exit_status = status;
}

int	get_current_exit_status(void)
{
	return (g_shell_exit_status);
}
