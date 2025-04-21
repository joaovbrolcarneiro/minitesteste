/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/21 22:43:32 by jbrol-ca         ###   ########.fr       */
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

static int count_following_words(t_token *start_node, t_token *end_token)
{
    t_token *current;
    int count = 0;

    if (!start_node) return 0;

    current = start_node->next;
    while (current && current != end_token && !current->used)
    {
       
        if (current->type == TOKEN_PIPE || current->coretype == REDIR)
            break;

        if (current->type == TOKEN_WORD) {
            count++;
        }

        current = current->next;
    }
    return count;
}


static char **gather_arguments(t_token *cmd_token, t_token *end_token)
{
    char    **args = NULL;
    t_token *current;
    int     arg_count;
    int     i = 0;

    if (!cmd_token || !cmd_token->value) return NULL;
    arg_count = count_following_words(cmd_token, end_token);
    args = hb_malloc(sizeof(char *) * (arg_count + 2));
    if (!args) {
        perror("konosubash: gather_arguments: malloc failed");
        return NULL;
    }

    args[0] = cmd_token->value;
    i = 1;

    current = cmd_token->next;
    while (current && current != end_token && !current->used && i <= arg_count)
    {
        if (current->type == TOKEN_PIPE || current->coretype == REDIR)
            break;

        if (current->type == TOKEN_WORD)
        {

            args[i] = ft_strdup(current->value);
            if (!args[i]) {
                 perror("konosubash: gather_arguments: strdup failed");
                 while (--i >= 1) free(args[i]);
                 free(args);
                 return NULL;
            }
            current->used = true;
            i++;
        }
        current = current->next;
    }

    args[i] = NULL;

    return args;
}


static char *gather_filename(t_token *redir_token, t_token *end_token)
{
    t_token *file_token;
    char *filename = NULL;

    if (!redir_token) return NULL;

    file_token = redir_token->next;

    while (file_token && file_token != end_token && file_token->used) {
        file_token = file_token->next;
    }


    if (file_token && file_token != end_token && file_token->type == TOKEN_WORD && !file_token->used)
    {
        filename = ft_strdup(file_token->value);
        if (!filename) {
             perror("konosubash: gather_filename: strdup failed");
             return NULL;
        }
        file_token->used = true;
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