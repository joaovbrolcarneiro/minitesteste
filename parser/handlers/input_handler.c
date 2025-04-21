/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:05:15 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 21:26:14 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"


t_token *delegated_by_input_handler(char *input, char **env)
{
    t_token *first;
    t_token *token;

    token = split_input(input, 0);
    first = token;
    if (!first)
        return (NULL);

    // Pipeline: Quotes -> Concatenate Quotes -> Types
    quote_handler_call_loop(token, env);

    // --- Perform adjacent quote/word concatenation ---
    perform_quote_concatenation(first); // <<< ADD THIS CALL
    // -----------------------------------------------

    typealize_call_loop(token, env);

    // Error check after quote handling/concatenation
    if (has_parser_error(first)) {
        return (first); // Let caller handle cleanup
    }

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
    t_node_tree *tree = NULL; // Initialize AST pointer to NULL
    bool        parse_error_flagged_in_tokens;

    // 1. Tokenization and initial processing (quotes, types, merge)
    token_list = delegated_by_input_handler(input, shell->env);

    // --- Handle NULL token_list (empty input or malloc failure) ---
    if (!token_list) {
        // g_exit_code might be 1 if split_input failed malloc
        return (NULL);
    }

    // 2. Check for parsing errors flagged *in* the token list itself (e.g., unclosed quotes)
    parse_error_flagged_in_tokens = has_parser_error(token_list); // Assumes checks token->err flags

    if (!parse_error_flagged_in_tokens)
    {
        // 3. Pipeline for valid token list: Assignments -> Expansion -> Command Type Check
        //    (Run BEFORE building the AST)
        process_variable_assignments(shell, token_list);
        expand_token_list_no_assignments(token_list, shell->env);
        parser_cmd_no_found(token_list, shell->env); // Check command validity/type *before* AST build

        // Check again for errors after expansion/cmd check if they can fail
        parse_error_flagged_in_tokens = has_parser_error(token_list); // Re-check if steps above set errors
    }

    // 4. Build the AST if no errors found so far
    if (!parse_error_flagged_in_tokens)
    {
        // Reset token 'used' flags before building AST for this command line
        t_token *temp = token_list;
        while(temp) {
            temp->used = false;
            temp = temp->next;
        }
        // g_exit_code = 0; // Optionally reset before parsing attempt

        tree = init_yggdrasil(token_list); // Build the AST

        // init_yggdrasil returns NULL on syntax error (like missing redirect file)
        // and sets g_exit_code.
        if (tree)
        {
            // 5. Execute the AST if successfully built
            shell->ast_root = tree; // Store tree in shell struct
            execute_ast(shell, shell->ast_root); // Execute
            // g_exit_code is set by execute_ast

            // 6. Free the AST after execution (Responsibility moved potentially)
            // free_ast(shell->ast_root); // Maybe called later in main loop?
            shell->ast_root = NULL; // Clear pointer after freeing
        }
        else if (g_exit_code == 0) {
             // If tree is NULL but no error code, command might be empty/non-executable.
             // Valid state, exit code remains 0 (or from last command).
        }
        // If tree is NULL and g_exit_code != 0, syntax error occurred during init_yggdrasil.
        // Error already printed.
    }
    else
    {
        // Error flagged earlier (e.g., unclosed quotes).
        g_exit_code = 2; // Ensure syntax error code
        // Error message should have been printed already.
    }

    // Return the original token list head for cleanup by the caller (readline_loop)
    // Even if parsing/execution failed, the list needs freeing.
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