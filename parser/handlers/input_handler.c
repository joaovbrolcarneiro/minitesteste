/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:05:15 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/23 21:12:21 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"


t_token	*delegated_by_input_handler(char *input, char **env)
{
	t_token	*first;

	first = split_input(input, 0);
	if (!first)
		return (NULL);
	quote_handler_call_loop(first, env);
	typealize_call_loop(first, env);
	perform_quote_concatenation(first);
	if (has_parser_error(first))
		return (first);
	return (first);
}


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

t_token *input_handler(t_shell *shell, char *input)
{
    t_token     *token_list;
    t_node_tree *tree = NULL; 
    bool        parse_error_flagged_in_tokens;

    token_list = delegated_by_input_handler(input, shell->env);

    if (!token_list) {
        return (NULL);
    }

    parse_error_flagged_in_tokens = has_parser_error(token_list);

    if (!parse_error_flagged_in_tokens)
    {
        process_variable_assignments(shell, token_list);
        expand_token_list_no_assignments(token_list, shell->env);
        parser_cmd_no_found(token_list, shell->env);

        parse_error_flagged_in_tokens = has_parser_error(token_list);
    }

    if (!parse_error_flagged_in_tokens)
    {
        t_token *temp = token_list;
        while(temp) {
            temp->used = false;
            temp = temp->next;
        }
	    print_token_lst(token_list);
        tree = init_yggdrasil(token_list);
	    print_yggdrasil(tree, 0, "root:");

        if (tree)
        {
            shell->ast_root = tree;
            execute_ast(shell, shell->ast_root);
            shell->ast_root = NULL;
        }
    }
    else
    {
        set_current_exit_status(2);
    }
    return (token_list);
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