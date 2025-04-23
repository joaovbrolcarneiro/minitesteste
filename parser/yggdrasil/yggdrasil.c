/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/23 21:26:02 by jbrol-ca         ###   ########.fr       */
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

t_token	*find_left_token(t_token *target_node, t_token *first_in_segment)
{
	t_token		*current_token;
	t_token		*last_highest_found;
	t_ranking	max_rank_found;

	if (target_node == first_in_segment || !target_node || !first_in_segment)
		return (NULL);
	last_highest_found = NULL;
	max_rank_found = RANK_F;
	current_token = get_prev_node(target_node, first_in_segment);
	while (current_token != NULL)
	{
		if (!current_token->used)
		{
			if (current_token->rank >= max_rank_found)
			{
				max_rank_found = current_token->rank;
				last_highest_found = current_token;
			}
		}
		if (current_token == first_in_segment)
			break ;
		current_token = get_prev_node(current_token, first_in_segment);
	}
	if (!last_highest_found)
		return (NULL);
	if (last_highest_found->rank == RANK_B)
	{
		current_token = first_in_segment;
		while (current_token && current_token != target_node)
		{
			if (current_token->type == TOKEN_CMD && !current_token->used)
				return (current_token);
			current_token = current_token->next;
		}
	}
	return (last_highest_found);
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

static int count_following_words(t_token *cmd_token)
{
    t_token *current;
    int count = 0;

    if (!cmd_token) return 0;
    current = cmd_token->next;
    while (current) {
        if (current->type == TOKEN_PIPE)
            break;
        if (current->coretype == REDIR) 
        {
            t_token *filename = current->next;
            if (filename && filename->type == TOKEN_WORD) 
            {
                current = filename->next;
                continue;
            } else { break; }
        }
        count++;
        current = current->next;
    }
    return count;
}


// --- Modified gather_arguments ---
// Scans for arguments until the next pipe or end of list, skipping over
// redirections and their filenames. Respects the 'used' flag set by gather_filename.
static char **gather_arguments(t_token *cmd_token, t_token *segment_end_token)
{
     (void)segment_end_token; 
    char    **args = NULL;
    t_token *current;
    int     arg_count_total;
    int     arg_capacity;
    int     i = 0;
    char    **temp_realloc = NULL;

    if (!cmd_token || !cmd_token->value) return NULL;

    arg_count_total = count_following_words(cmd_token);

    arg_capacity = arg_count_total + 1;
    args = hb_malloc(sizeof(char *) * (arg_capacity + 1));
    if (!args) { perror("malloc"); set_current_exit_status(1); return NULL; }

    args[0] = cmd_token->value;
    i = 1;
    args[i] = NULL;

    current = cmd_token->next;
    while (current) {
        if (current->type == TOKEN_PIPE) { break; }

        if (current->coretype == REDIR) {
            t_token *filename_token = current->next;
            if (filename_token && filename_token->type == TOKEN_WORD) 
            {
                 current = filename_token->next;
                 continue;
            } else 
            { 
                break; 
            }
        }

        if (!current->used) {
            if (i >= arg_capacity) 
            { 
                 arg_capacity = i + 5;
                 temp_realloc = realloc(args, sizeof(char *) * (arg_capacity + 1));
                 if (!temp_realloc) { perror("realloc"); while (--i >= 1) free(args[i]); free(args); set_current_exit_status(1); return NULL; }
                 args = temp_realloc;
            }
            args[i] = ft_strdup(current->value); // Add the token's value
            if (!args[i]) { perror("strdup"); while (--i >= 1) free(args[i]); free(args); set_current_exit_status(1); return NULL; }
            current->used = true;
            i++;
            args[i] = NULL;
        }
        current = current->next;
    }
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
            set_current_exit_status(1); // Signal error
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

t_node_tree	*make_yggdrasil(t_token *t, t_token *f, t_token *e, \
								t_node_tree *parent_y)
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;

	(void)parent_y;
	y = NULL;
	left_child_token = NULL;
	right_child_token = NULL;
	if (!t || t == e || t->used)
		return (NULL);
	y = new_yggnode(t); // Creates node based on token t, marks t used
	if (!y)
	{
		set_current_exit_status(1); // Use setter
		return (NULL);
	}
	// Check and gather args/file based on initial node type
	// Note: gather_arguments should be the version that skips redirections
	if (y->type == AST_COMMAND)
	{
		y->args = gather_arguments(t, NULL);
		if (!y->args && get_current_exit_status() != 0) // Use getter
		{
			free(y); // free?
			return (NULL);
		}
	}
	else if (y->type == AST_REDIR_IN || y->type == AST_REDIR_OUT
		|| y->type == AST_APPEND || y->type == AST_HEREDOC)
	{
		y->file = gather_filename(t, e);
		if (!y->file) // Check if filename missing or gather_filename failed
		{
			// If gather_filename failed malloc, it should set status code
			if (get_current_exit_status() == 0) // Use getter
			{
				st_prsr_err("syntax error near unexpected token", t->value);
				set_current_exit_status(2); // Use setter
			}
			// If y->args was somehow allocated for a redir node, free it
			if (y->args)
				ft_free_strarray(y->args); // free?
			free(y); // free?
			return (NULL);
		}
	}
	// --- Always find potential children and recurse ---
	left_child_token = find_left_token(t, f);
	right_child_token = find_right_token(t, e);
	y->left = make_yggdrasil(left_child_token, f, t, y);
	if (get_current_exit_status() != 0 && y->left == NULL \
		&& left_child_token && !left_child_token->used) // Use getter
	{
		if (y->args) ft_free_strarray(y->args); // Cleanup args if present
		if (y->file) free(y->file);             // Cleanup file if present
		free(y);
		return (NULL);
	}
	y->right = make_yggdrasil(right_child_token, t, e, y);
	if (get_current_exit_status() != 0 && y->right == NULL \
		&& right_child_token && !right_child_token->used) // Use getter
	{
		if (y->left) //free_ast(y->left);            // free? Assuming free_ast exists
		if (y->args) ft_free_strarray(y->args);
		if (y->file) free(y->file);
		free(y);
		return (NULL);
	}
	return (y);
}