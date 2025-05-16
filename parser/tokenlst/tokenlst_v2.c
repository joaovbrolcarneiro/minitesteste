/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenlst_v2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:05:42 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 18:46:43 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

/*bool	proximity_exception(char *input, int i)
{
	if ((!ischarset("|<>", input[i]) && input[i] != ' ') && \
	input[i] && !ischarset("|<>", *input))
		return (true);
	return (false);
}*/

/////////////////////////////////////////////// MEXI NESSA FUNCAO

t_token	*typealize(t_token *token, char **env)
{
	if (token->type == TOKEN_ASSIGNMENT)
		return (token);
	if (is_pipe_super(token->value))
		return (new_token_super(new_pipe(token)));
	if (is_redir_super(token->value))
		return (new_token_super(new_redir(token)));
	if (is_cmd_super(token->value, env))
		return (new_token_super(new_cmd(token)));
	if (is_word_super(token->value))
		return (new_token_super(new_word(token)));
	if (is_eof(token->value))
		return (new_token_super(new_eof(token)));
	return (token);
}

bool	is_redirection(t_obj_ygg obj)
{
	return (obj.y->type == AST_REDIR_IN
		|| obj.y->type == AST_REDIR_OUT
		|| obj.y->type == AST_APPEND
		|| obj.y->type == AST_HEREDOC);
}

void	handle_eof(t_shell *shell)
{
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	cleanup_shell(shell);
	exit(get_current_exit_status());
}

void	setup_interactive_signals(void)
{
	signal(SIGINT, handle_ctrl_c);
	signal(SIGQUIT, SIG_IGN);
}
