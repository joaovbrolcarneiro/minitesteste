/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 22:40:16 by jbrol-ca         ###   ########.fr       */
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

    // Basic validation
    if (!token)
        return (NULL);

    // Critical: Check if token is already used in the AST
    if (token->used == true)
        return (NULL); // Don't create a node for a token already used

    new_node = hb_malloc(sizeof(t_node_tree));
    if (!new_node) {
        perror("konosubash: new_yggnode: hb_malloc failed");
        return (NULL);
    }

    // Mark the token as used *now* that we are building its AST node
    token->used = true;

    // Initialize node properties
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->content = token->value;      // Keep pointer to original token value
    new_node->type = (t_ast_type)token->type; // Map token type to AST type
    new_node->rank = token->rank;          // Copy rank (may or may not be needed in AST)

    // --- Important Change ---
    // Initialize args and file to NULL. They will be populated
    // by the make_yggdrasil logic if this node is a command or redirection.
    new_node->args = NULL;
    new_node->file = NULL;
    // ------------------------

    // Copy other relevant fields from token if needed by executor
    // (Consider if these are truly needed in the AST or just the token)
    // new_node->used = token->used; // Node existence implies token usage
    new_node->err = token->err;
    new_node->coretype = token->coretype;
    new_node->literal = token->literal;
    // new_node->merge_next = token->merge_next; // Unlikely needed in AST

    return (new_node);
}

static int count_following_words(t_token *start_node, t_token *end_token)
{
    t_token *current;
    int count = 0;

    if (!start_node) return 0;

    current = start_node->next;
    while (current && current != end_token && !current->used)
    {
        // Stop if we hit a token that breaks a command sequence (pipe/redir)
        // Use coretype if it reliably distinguishes operators from words after typing.
        if (current->type == TOKEN_PIPE || current->coretype == REDIR)
            break;

        // Count only words that are not yet part of the AST
        if (current->type == TOKEN_WORD) {
            count++;
        }
        // It's debatable whether non-word tokens should stop the count.
        // Bash treats 'cmd > file arg' - arg belongs to cmd.
        // Let's assume for now only pipes/redirs break the sequence.
        // If a non-word token is encountered that isn't pipe/redir,
        // it might be a syntax error or handled differently depending on shell rules.

        current = current->next;
    }
    return count;
}


// Helper to gather arguments for an AST_COMMAND node
// Returns a NULL-terminated array of strings (char **)
// Marks the consumed TOKEN_WORD tokens as used.
// Handles memory allocation. Returns NULL on failure.
static char **gather_arguments(t_token *cmd_token, t_token *end_token)
{
    char    **args = NULL;
    t_token *current;
    int     arg_count;
    int     i = 0;

    if (!cmd_token || !cmd_token->value) return NULL; // Should not happen if called correctly

    // 1. Count the number of arguments (TOKEN_WORDs) following cmd_token
    arg_count = count_following_words(cmd_token, end_token);

    // 2. Allocate the args array (+1 for command name, +1 for NULL terminator)
    args = hb_malloc(sizeof(char *) * (arg_count + 2));
    if (!args) {
        perror("konosubash: gather_arguments: malloc failed");
        return NULL;
    }

    // 3. First argument is the command name itself
    args[0] = cmd_token->value; // Point to the command token's value
    i = 1;

    // 4. Iterate again, copying word values and marking tokens used
    current = cmd_token->next;
    while (current && current != end_token && !current->used && i <= arg_count)
    {
        if (current->type == TOKEN_PIPE || current->coretype == REDIR)
            break;

        if (current->type == TOKEN_WORD)
        {
            // It's generally safer to duplicate argument strings
            args[i] = ft_strdup(current->value);
            if (!args[i]) {
                 perror("konosubash: gather_arguments: strdup failed");
                 while (--i >= 1) free(args[i]); // Free already strdup'd args
                 free(args);
                 return NULL; // Allocation failure
            }
            current->used = true; // Mark this token as used (as an argument)
            i++;
        }
        current = current->next;
    }

    // 5. NULL-terminate the array
    args[i] = NULL;

    return args;
}

// Helper to get the filename for a redirection node
// Returns a pointer to the filename string (duplicated for safety).
// Marks the consumed TOKEN_WORD token as used.
// Returns NULL if no valid filename token is found or on allocation error.
static char *gather_filename(t_token *redir_token, t_token *end_token)
{
    t_token *file_token;
    char *filename = NULL;

    if (!redir_token) return NULL;

    file_token = redir_token->next;

    // Look for the next token that is a WORD and not used
    while (file_token && file_token != end_token && file_token->used) {
        file_token = file_token->next; // Skip already used tokens
    }

    // Check if we found a valid, unused word token
    if (file_token && file_token != end_token && file_token->type == TOKEN_WORD && !file_token->used)
    {
        // Duplicate the filename for safety
        filename = ft_strdup(file_token->value);
        if (!filename) {
             perror("konosubash: gather_filename: strdup failed");
             return NULL;
        }
        file_token->used = true; // Mark the filename token as used
    }
    else
    {
        // Error: Missing name for redirect or unexpected token
        // We signal this by returning NULL. The caller (`make_yggdrasil` or executor)
        // should handle this syntax error.
        // Optionally, print error here, but better to do it centrally.
        // st_prsr_err("syntax error near unexpected token", redir_token->value);
        // g_exit_code = 2; // Set syntax error code
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
    // --- Fix: Mark parent_y as intentionally unused ---
    (void)parent_y;
    // -------------------------------------------------

    t_node_tree *y = NULL; // Current node being processed
    t_token *left_child_token = NULL;
    t_token *right_child_token = NULL;

    // --- Base Case or Skip Token ---
    if (!t || t == e || t->used) {
        return NULL;
    }

    // --- Create Node ---
    y = new_yggnode(t);
    if (!y) {
        g_exit_code = 1;
        return NULL;
    }

    // --- Gather Arguments or Filename (if applicable) ---
    if (y->type == AST_COMMAND) {
        y->args = gather_arguments(t, e);
        if (!y->args && g_exit_code != 0) { // Check if gather_arguments failed malloc
             free(y); // Free the node itself if args failed
             return NULL;
        }
    } else if (y->type == AST_REDIR_IN || y->type == AST_REDIR_OUT ||
               y->type == AST_APPEND || y->type == AST_HEREDOC) {
        y->file = gather_filename(t, e);
        if (!y->file) {
             st_prsr_err("syntax error near unexpected token", t->value);
             g_exit_code = 2;
             // Free the node 'y' created before the error was found
             // simplistic free: assumes no args allocated for redir node
             if (y->args) ft_free_strarray(y->args); // Just in case
             if (y->content) { /* Decide if content needs freeing */ }
             free(y);
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