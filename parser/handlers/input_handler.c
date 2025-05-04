/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:05:15 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 15:50:47 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"
/*
t_token *delegated_by_input_handler(char *input, char **env) -- FUNCAO ANTIGA
{
	t_token *first;
	t_token *token;

	if( is_too_long_input_exption(input))
		return (NULL);
	token = split_input(input, 0);
	first = token;
	quote_handler_call_loop(token, env);
	typealize_call_loop(token, env);
	merg_tok_excep_cll_revloop(token);
	return (token);
}*/

t_inpt_hndlr	input_handler_part2(t_shell *shell, char *input)
{
	t_token		*token_list;
	t_node_tree	*tree;
	bool		parse_error_flagged_in_tokens;
	t_token		*temp;

	tree = NULL;
	token_list = delegated_by_input_handler(input, shell->env);
	if (!token_list)
		token_list = NULL;
	parse_error_flagged_in_tokens = has_parser_error(token_list);
	temp = NULL;
	return ((t_inpt_hndlr){token_list, tree, \
	parse_error_flagged_in_tokens, temp});
}

void	input_handler_part3(t_shell *shell, t_token *token_list)
{
	process_variable_assignments(shell, token_list);
	expand_token_list_no_assignments(token_list, shell->env);
	parser_cmd_no_found(token_list, shell->env);
}

void	input_handler_part4(t_inpt_hndlr *cu)
{
	while (cu->temp)
	{
		cu->temp->used = false;
		cu->temp = cu->temp->next;
	}
}

t_token	*input_handler(t_shell *shell, char *input)
{
	t_inpt_hndlr	cu;

	cu = input_handler_part2(shell, input);
	if (!cu.token_list)
		return (NULL);
	if (!cu.parse_error_flagged_in_tokens)
	{
		input_handler_part3(shell, cu.token_list);
		cu.parse_error_flagged_in_tokens = has_parser_error(cu.token_list);
	}
	if (!cu.parse_error_flagged_in_tokens)
	{
		cu.temp = cu.token_list;
		input_handler_part4(&cu);
		print_token_lst(cu.token_list);
		cu.tree = init_yggdrasil(cu.token_list);
		print_yggdrasil(cu.tree, 0, "root:");
		if (cu.tree)
		{
			shell->ast_root = cu.tree;
			execute_ast(shell, shell->ast_root);
			shell->ast_root = NULL;
		}
	}
	else
		set_current_exit_status(2);
	return (cu.token_list);
}
/*void input_handler(char **env, char *input) ---- FUNCAO ANTIGA
{
	t_token *token;
	t_node_tree *tree;

	token = delegated_by_input_handler(input, env);
	if(!token)
		return ;
	token = delegated_by_input_handler(input, env);
	token = handler_args_file(token, token);
	parser_cmd_no_found(token, env);
	//print_token_lst(token);
	tree = init_yggdrasil(token);
	//print_yggdrasil(tree, 0, "root:");
}*/
