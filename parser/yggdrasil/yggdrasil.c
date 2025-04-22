/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/22 22:13:54 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_token *find_right_token(t_token *token, t_token *eof)
{
	t_token *father;
	int i;

	i = 0;
	if(!token)
		return(NULL);
	if(token->next == eof || !token->next || token == eof)
		return (NULL);
	father = token;
	token = token->next;
	while (1)
	{
		if(father->rank - i == token->rank)
			return (token);
		if (father->rank - i == RANK_F)
			return (NULL);
		token = token->next;
		if(token == eof)
		{
			i++;
			token = father->next;
		}
	}
	return (token);
}

t_token *find_left_token(t_token *token, t_token *first)
{
	t_token *eof;
	int i;

	i = 0;
	if (token == first || !token || !first)
		return(NULL);
	eof = token;
	token = get_prev_node(token, first);
	while (1)
	{
		if(eof->rank - i == token->rank)
			return (token);
		//?????????
		/*if(eof->rank - i < token->rank )
			return (NULL);*/
		//?????????
		if (eof->rank - i == RANK_F)
			return (NULL);
		token = get_prev_node(token, first);
		if(token == NULL)
		{
			i++;
			token = get_prev_node(eof, first);
		}
	}
	return (token);
}

t_node_tree *new_yggnode(t_token *token)
{
    t_node_tree *new_node;

    if (!token)
        return (NULL);

    if (token->used == true)
        return (NULL);
    new_node = hb_malloc(sizeof(t_node_tree));
    if (!new_node) {
        perror("konosubash: new_yggnode: hb_malloc failed");
        return (NULL);
    }

    token->used = true;

    new_node->left = NULL;
    new_node->right = NULL;
    new_node->content = token->value;      
    new_node->type = (t_ast_type)token->type;
    new_node->rank = token->rank;          

    
    new_node->args = NULL;
    new_node->file = NULL;
    // ------------------------

   
    new_node->err = token->err;
    new_node->coretype = token->coretype;
    new_node->literal = token->literal;
    // new_node->merge_next = token->merge_next; // Unlikely needed in AST

    return (new_node);
}

static int count_following_words(t_token *cmd_token /*, t_token *segment_end_token - No longer needed */)
{
    t_token *current;
    int count = 0;

    if (!cmd_token) return 0;

    current = cmd_token->next; // Start scanning after the command token

    // Scan until the true end (pipe or end of all tokens)
    while (current)
    {
        // Stop definitively if a pipe is encountered
        if (current->type == TOKEN_PIPE) {
            break;
        }

        // If it's a redirection operator...
        if (current->coretype == REDIR) {
            t_token *filename = current->next;
            // Check if a valid filename follows (must exist and be WORD)
            if (filename && filename->type == TOKEN_WORD) {
                current = filename->next; // Position current AFTER the filename
                continue; // Continue scan from the token after the filename
            } else {
                // Malformed redirection (missing/invalid filename)
                // Stop counting here as the command structure is likely invalid
                break;
            }
        }

        // If it's a word, increment count
        // We count *potential* args; gather_arguments will check 'used' flag.
        if (current->type == TOKEN_WORD) {
            count++;
        }
        // If it's not PIPE, REDIR, or WORD, just skip it

        current = current->next;
    } // End while loop

    return count;
}


// --- Modified gather_arguments ---
// Scans for arguments until the next pipe or end of list, skipping over
// redirections and their filenames. Respects the 'used' flag set by gather_filename.
static char **gather_arguments(t_token *cmd_token, t_token *segment_end_token)
{
    // Note: segment_end_token passed from make_yggdrasil might still be useful
    //       to know the original recursive boundary, but the scan logic now
    //       primarily stops at pipes or NULL. Let's ignore it for now.
     (void)segment_end_token; // Mark as unused for this logic

    char    **args = NULL;
    t_token *current;
    int     arg_count_total;
    int     arg_capacity;
    int     i = 0; // Index for filling args array
    char    **temp_realloc = NULL;

    if (!cmd_token || !cmd_token->value) return NULL;

    // 1. Count ALL potential arguments until the next pipe or end of list
    arg_count_total = count_following_words(cmd_token /*, NULL */); // Use modified counter

    // 2. Allocate args array (command + counted args + NULL terminator)
    arg_capacity = arg_count_total + 1;
    args = hb_malloc(sizeof(char *) * (arg_capacity + 1));
    if (!args) {
        perror("konosubash: gather_arguments: malloc failed");
        g_exit_code = 1; // Signal error
        return NULL;
    }

    // 3. Add command name as args[0]
    // Important: We point to the original token value. Executor must not free args[0].
    args[0] = cmd_token->value;
    i = 1;
    args[i] = NULL; // Keep NULL terminated

    // 4. Iterate through tokens after command, collecting unused WORDs
    current = cmd_token->next;
    while (current) // Scan until end of list
    {
        // Stop definitively at a pipe
        if (current->type == TOKEN_PIPE) {
            break;
        }

        // If it's a redirection operator... skip it and its filename
        if (current->coretype == REDIR) {
            t_token *filename_token = current->next;
            // Check for valid filename token (must exist and be WORD)
            // gather_filename called by make_yggdrasil would have already marked
            // the filename used and potentially errored if missing/invalid.
            if (filename_token && filename_token->type == TOKEN_WORD) {
                 current = filename_token->next; // Continue scan AFTER filename
                 continue;
            } else {
                 // Malformed redirection encountered during arg scan. Stop.
                 // (Parser should ideally catch this earlier via gather_filename failing)
                 break;
            }
        }

        // If it's a WORD token AND it hasn't been used (e.g., as a filename by gather_filename)
        if (current->type == TOKEN_WORD && !current->used) {
            // Ensure capacity (safer in case count was off)
            if (i >= arg_capacity) {
                arg_capacity = i + 5; // Increase capacity
                // Use standard realloc (or your hb_realloc equivalent)
                temp_realloc = realloc(args, sizeof(char *) * (arg_capacity + 1));
                if (!temp_realloc) {
                    perror("konosubash: gather_arguments: realloc failed");
                    while (--i >= 1) free(args[i]); // Free strdup'd strings
                    free(args); // Free the array itself
                    g_exit_code = 1;
                    return NULL;
                }
                args = temp_realloc;
            }

            // Duplicate the argument value
            args[i] = ft_strdup(current->value);
            if (!args[i]) {
                perror("konosubash: gather_arguments: strdup failed");
                while (--i >= 1) free(args[i]);
                free(args);
                g_exit_code = 1;
                return NULL;
            }
            current->used = true; // Mark this token as used (as an argument)
            i++;
            args[i] = NULL; // Keep NULL terminated
        }
        // If token is not PIPE, REDIR, or usable WORD, just skip it

        current = current->next; // Move to next token
    } // End while loop

    // 5. Final args array is ready
    return args;
}

// --- gather_filename remains unchanged ---
// It correctly finds the next unused WORD token after the redirection operator
// and marks it used. The modified gather_arguments respects this.
static char *gather_filename(t_token *redir_token, t_token *end_token)
{
    t_token *file_token;
    char *filename = NULL;

    if (!redir_token) return NULL;

    file_token = redir_token->next;

    // Skip already used tokens to find filename
    while (file_token && file_token != end_token && file_token->used) {
        file_token = file_token->next;
    }


    // Check if we found a valid, unused word token
    if (file_token && file_token != end_token && file_token->type == TOKEN_WORD && !file_token->used)
    {
        // Duplicate the filename for safety
        filename = ft_strdup(file_token->value);
        if (!filename) {
            perror("konosubash: gather_filename: strdup failed");
            g_exit_code = 1; // Signal error
            return NULL;
        }
        file_token->used = true; // Mark the filename token as used
    }
    else
    {
        // Filename missing or invalid, filename remains NULL
        // make_yggdrasil should check for NULL and report syntax error
    }

    return filename;
}
/*funcao antiga
t_node_tree *new_yggnode(t_token *token)
{
    t_node_tree *new_node;

    new_node = hb_malloc(sizeof(t_node_tree));
    if (!new_node)
	{
        return (NULL);
	}
	if (!token)
        return (NULL);
	if (token->used == true)
		return (NULL);
	token->used = true;
	new_node->right = NULL;
	new_node->left = NULL;
	if (!new_node->right || !new_node->left)
		return (NULL);
	new_node->content = token->value;
	new_node->type = (t_ast_type)token->type;
	new_node->rank = token->rank;
	new_node->args = token->args;
	new_node->file = token->file;
    return (new_node);
}*/

t_node_tree *make_yggdrasil(t_token *t, t_token *f, t_token *e, t_node_tree *parent_y)
{
    (void)parent_y;

    t_node_tree *y = NULL;
    t_token *left_child_token = NULL;
    t_token *right_child_token = NULL;

    if (!t || t == e || t->used) {
        return NULL;
    }

    y = new_yggnode(t);
    if (!y) {
        g_exit_code = 1;
        return NULL;
    }

    if (y->type == AST_COMMAND) {
        y->args = gather_arguments(t, e);
        if (!y->args && g_exit_code != 0) {
             free(y); //free?
             return NULL;
        }
    } else if (y->type == AST_REDIR_IN || y->type == AST_REDIR_OUT ||
               y->type == AST_APPEND || y->type == AST_HEREDOC) {
        y->file = gather_filename(t, e);
        if (!y->file) {
             st_prsr_err("syntax error near unexpected token", t->value);
             g_exit_code = 2;
             if (y->args) ft_free_strarray(y->args); // free?
             if (y->content)
             free(y); //free?
             return NULL;
        }
    }

    // --- Find Potential Children ---
    left_child_token = find_left_token(t, f);
    right_child_token = find_right_token(t, e);

    // --- Recursive Calls ---

    y->left = make_yggdrasil(left_child_token, f, t, y);
    if (g_exit_code != 0 && y->left == NULL && left_child_token && !left_child_token->used) {
         if(y->args) ft_free_strarray(y->args);
         if(y->file) free(y->file);
         free(y);
         return NULL;
    }

    y->right = make_yggdrasil(right_child_token, t, e, y);
    if (g_exit_code != 0 && y->right == NULL && right_child_token && !right_child_token->used) {
         if (y->left) { /* free_ast(y->left); free?*/ }
         if(y->args) ft_free_strarray(y->args);
         if(y->file) free(y->file);
         free(y);
         return NULL;
    }
    return y;
}