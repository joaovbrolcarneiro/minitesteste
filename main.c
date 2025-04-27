/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* main.c                                             :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/04/15 00:00:00 by hde-barr          #+#    #+#             */
/* Updated: 2025/04/15 00:00:01 by hde-barr         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

static int g_shell_exit_status; 

/*
** konopwd:
** - If the trimmed input equals "pwd", prints the current directory.
*/

void	typealize_call_loop(t_token *token, char **env)
{
	t_token	*current_token;

	current_token = token;
	while (current_token)
	{
		typealize(current_token, env);
		current_token = current_token->next;
	}
}

void merg_tok_excep_cll_revloop(t_token *token)
{
    (void)token;
}
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

/*
** main:
*/
int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	set_current_exit_status(0); // Use setter instead of g_exit_code = 0;
	init_shell(&shell, envp);
	signal(SIGINT, handle_ctrl_c);
	signal(SIGQUIT, SIG_IGN);
	readline_loop(&shell);
	cleanup_shell(&shell);
	return (get_current_exit_status()); // Use getter instead of return (g_exit_code);
}

bool	is_builtin(const char *cmd)
{
	if (!cmd)
		return (false);
	if (ft_strcmp(cmd, "echo") == 0 || \
		ft_strcmp(cmd, "cd") == 0 || \
		ft_strcmp(cmd, "pwd") == 0 || \
		ft_strcmp(cmd, "export") == 0 || \
		ft_strcmp(cmd, "unset") == 0 || \
		ft_strcmp(cmd, "exit") == 0)
	{
		return (true);
	}
	return (false);
}

void set_current_exit_status(int status)
{
    g_shell_exit_status = status;
}

int get_current_exit_status(void)
{
    return (g_shell_exit_status);
}