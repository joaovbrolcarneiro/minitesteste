/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 17:30:41 by jbrol-ca         ###   ########.fr       */
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

typedef struct s_flt_vrs
{
	t_token		*cu_tken;
	t_token		*last_highest_found;
	t_ranking	max_rank_found;
} t_flt_vrs;

bool flt_nrm(t_token *target_node, t_token *first_in_segment, t_flt_vrs *vrs)
{
	
	if (target_node == first_in_segment || !target_node || !first_in_segment)
		return (true);
	vrs->last_highest_found = NULL;
	vrs->max_rank_found = RANK_F;
	vrs->cu_tken = get_prev_node(target_node, first_in_segment);
	return(false);
}

void flt_nrm2(t_flt_vrs *vrs)
{
	vrs->max_rank_found = vrs->cu_tken->rank;
	vrs->last_highest_found = vrs->cu_tken;
}


t_token	*find_left_token(t_token *target_node, t_token *first_in_segment)
{
	t_flt_vrs vars;

	if (flt_nrm(target_node, first_in_segment, &vars))
		return (NULL);
	while (vars.cu_tken != NULL)
	{
		if (!vars.cu_tken->used && vars.cu_tken->rank >= vars.max_rank_found)
			flt_nrm2(&vars);
		if (vars.cu_tken == first_in_segment)
			break ;
		vars.cu_tken = get_prev_node(vars.cu_tken, first_in_segment);
	}
	if (!vars.last_highest_found)
		return (NULL);
	if (vars.last_highest_found->rank == RANK_B)
	{
		vars.cu_tken = first_in_segment;
		while (vars.cu_tken && vars.cu_tken != target_node)
		{
			if (vars.cu_tken->type == TOKEN_CMD && !vars.cu_tken->used)
				return (vars.cu_tken);
			vars.cu_tken = vars.cu_tken->next;
		}
	}
	return (vars.last_highest_found);
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
    // new_node->merge_next = token->merge_next; // acho que inutil

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

typedef struct s_gthr_arg_vrs
{
	char    **args;
    t_token *current;
    int     arg_count_total;
    int     arg_capacity;
    int     i;
    char    **temp_realloc;
}	t_gthr_arg_vrs;


t_gthr_arg_vrs gthr_arg_vrs_init(t_gthr_arg_vrs *cu)
{
	cu->args = NULL;
	cu->temp_realloc = NULL;
	cu->i = 0;
	return (*cu);
}

static bool gather_arg_helper(t_token *cmd_token, t_gthr_arg_vrs *cu)
{
	if (!cmd_token || !cmd_token->value) 
		return true;
    cu->arg_count_total = count_following_words(cmd_token);
    cu->arg_capacity = cu->arg_count_total + 1;
    cu->args = hb_malloc(sizeof(char *) * (cu->arg_capacity + 1));
	return (false);
}

static bool gather_arg_helper2(t_token *cmd_token, t_gthr_arg_vrs *cu)
{
	if (!cu->args) 
		return (set_current_exit_status(1), true);
    cu->args[0] = cmd_token->value;
    cu->i = 1;
    cu->args[cu->i] = NULL;
	return (false);
}

static bool gather_arg_helper3_part2(t_gthr_arg_vrs *cu)
{
    if (cu->i >= cu->arg_capacity)
    {
        size_t old_byte_size = sizeof(char *) * (cu->arg_capacity + 1);
        cu->arg_capacity = cu->i + 5;
        size_t new_byte_size = sizeof(char *) * (cu->arg_capacity + 1);

        cu->temp_realloc = ft_realloc(cu->args, old_byte_size, new_byte_size);

        if (!cu->temp_realloc)
            return (set_current_exit_status(1), true);
        cu->args = cu->temp_realloc;
    }
    return (false);
}

static bool gather_arg_helper3_part3(t_gthr_arg_vrs *cu)
{
    if (!cu->args[cu->i]) 
		return (set_current_exit_status(1), true);
	cu->current->used = true;
	cu->i++;
	cu->args[cu->i] = NULL;
	return (false);
}

static char **gather_arg_helper3(t_gthr_arg_vrs *cu)
{
	while (cu->current) 
	{
        if (cu->current->type == TOKEN_PIPE) 
			break;
        if (cu->current->coretype == REDIR) 
		{
            t_token *filename_token = cu->current->next;
            if (filename_token && filename_token->type == TOKEN_WORD) 
            {
                 cu->current = filename_token->next;
                 continue;
            }
            break; 
        }
        if (!cu->current->used) 
		{
			if(gather_arg_helper3_part2(cu))
				return (NULL);
            cu->args[cu->i] = ft_strdup(cu->current->value);
			if(gather_arg_helper3_part3(cu))
				return (NULL);
        }
        cu->current = cu->current->next;
    }
	return (cu->args);
}
// --- Modified gather_arguments ---
// Scans for arguments until the next pipe or end of list, skipping over
// redirections and their filenames. Respects the 'used' flag set by gather_filename.
static char **gather_arguments(t_token *cmd_token, t_token *segment_end_token)
{
    (void)segment_end_token;
	t_gthr_arg_vrs cu;
	
	cu = gthr_arg_vrs_init(&cu);
	if(gather_arg_helper(cmd_token, &cu))
		return (NULL);
	if(gather_arg_helper2(cmd_token, &cu))
		return (NULL);
    cu.current = cmd_token->next;
    cu.args = gather_arg_helper3(&cu);
    return (cu.args);
}

/* // gather_arguments original !!!!!!!
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
    while (current) 
	{
        if (current->type == TOKEN_PIPE) 
			break; 
        if (current->coretype == REDIR) 
		{
            t_token *filename_token = current->next;
            if (filename_token && filename_token->type == TOKEN_WORD) 
            {
                 current = filename_token->next;
                 continue;
            } 
			else 
                break; 
        }
        if (!current->used) 
		{
            if (i >= arg_capacity) 
            { 
                 arg_capacity = i + 5;
                 temp_realloc = realloc(args, sizeof(char *) * (arg_capacity + 1));
                if (!temp_realloc) 
				{ 
					perror("realloc"); 
					while (--i >= 1) 
						free(args[i]); 
					free(args); 
					set_current_exit_status(1); 
					return NULL; 
				}
                 args = temp_realloc;
            }
            args[i] = ft_strdup(current->value); // Add the token's value
            if (!args[i]) 
			{ 
				perror("strdup"); 
				while (--i >= 1) 
					free(args[i]); 
				free(args); 
				set_current_exit_status(1); 
				return NULL; 
			}
            current->used = true;
            i++;
            args[i] = NULL;
        }
        current = current->next;
    }
    return args;
}
 */

// --- gather_filename remains unchanged ---
// It correctly finds the next unused WORD token after the redirection operator
// and marks it used. The modified gather_arguments respects this.
static char *gather_filename(t_token *redir_token, t_token *end_token)
{
    t_token *file_token;
    char *filename = NULL;

    if (!redir_token) 
		return NULL;
    file_token = redir_token->next;
    // Skip already used tokens to find filename
    while (file_token && file_token != end_token && file_token->used) 
        file_token = file_token->next;
    // Check if we found a valid, unused word token
    if (file_token && file_token != end_token && file_token->type == TOKEN_WORD && !file_token->used)
    {
        // Duplicate the filename for safety
        filename = ft_strdup(file_token->value);
        if (!filename) 
		{
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




/*static t_node_tree	*make_yggdrasil_helper(t_token *t, t_token *f, \
t_token *e, t_node_tree *parent_y)
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;
	
	(void)parent_y;
	y = NULL;
	
}*/

typedef struct s_obj_ygg
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;
}	t_obj_ygg;


static t_obj_ygg	make_yggdrasil_init(void)
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;

	y = NULL;
	left_child_token = NULL;
	right_child_token = NULL;
	
	return ((t_obj_ygg){y, left_child_token, right_child_token});
	
}

static bool r(t_obj_ygg obj_ygg)
{
	if (obj_ygg.right_child_token && !obj_ygg.right_child_token->used)
		return (true);
	return (false);
}

static bool l(t_obj_ygg obj_ygg)
{
	if (obj_ygg.left_child_token && !obj_ygg.left_child_token->used)
		return (true);
	return (false);
}

static void by_pass_st_prsr_err(t_token *t)
{
	st_prsr_err("syntax error near unexpected token", t->value), \
	set_current_exit_status(2);
}

static bool by_pass_redir(t_obj_ygg ob_yg)
{
	if (ob_yg.y->type == AST_REDIR_IN || ob_yg.y->type == AST_REDIR_OUT
		|| ob_yg.y->type == AST_APPEND || ob_yg.y->type == AST_HEREDOC)
		return (true);
	return (false);
}

//function default proprety
static bool fdp(t_obj_ygg ob_yg, t_token *t, t_token *f, t_token *e)
{
	ob_yg.left_child_token = find_left_token(t, f);
	ob_yg.right_child_token = find_right_token(t, e);
	ob_yg.y->left = mke_yggdrasil(ob_yg.left_child_token, f, t, ob_yg.y);
	if (get_current_exit_status() != 0 && ob_yg.y->left == NULL && l(ob_yg))
		return (true);
	ob_yg.y->right = mke_yggdrasil(ob_yg.right_child_token, t, e, ob_yg.y);
	if (get_current_exit_status() != 0 && ob_yg.y->right == NULL && r(ob_yg))
		return (true);
	return (false);
}


t_node_tree	*mke_yggdrasil(t_token *t, t_token *f, t_token *e, \
t_node_tree *parent_y)
{
	t_obj_ygg	ob_yg;

	(void)parent_y;
	ob_yg = make_yggdrasil_init();

	if (!t || t == e || t->used)
		return (NULL);
	ob_yg.y = new_yggnode(t);
	if (!ob_yg.y)
		return (set_current_exit_status(1) ,NULL);
	if (ob_yg.y->type == AST_COMMAND)
	{
		ob_yg.y->args = gather_arguments(t, NULL);
		if (!ob_yg.y->args && get_current_exit_status() != 0)
			return (NULL);
	}
	else if (by_pass_redir(ob_yg))
	{
		ob_yg.y->file = gather_filename(t, e);
		if (!ob_yg.y->file)
			return (by_pass_st_prsr_err(t), NULL);
	}
	if(fdp(ob_yg, t, f, e))
		return (NULL);
	return (ob_yg.y);
}



/* Versao 2!!!!
typedef struct s_obj_ygg
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;
}	t_obj_ygg;


static t_obj_ygg	make_yggdrasil_init(void)
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;

	y = NULL;
	left_child_token = NULL;
	right_child_token = NULL;
	
	return ((t_obj_ygg){y, left_child_token, right_child_token});
	
}



t_node_tree	*make_yggdrasil(t_token *t, t_token *f, t_token *e, \
t_node_tree *parent_y)
{
	t_obj_ygg obj_ygg;

	(void)parent_y;
	obj_ygg = make_yggdrasil_init();

	if (!t || t == e || t->used)
		return (NULL);
	obj_ygg.y = new_yggnode(t);
	if (!obj_ygg.y)
		return (set_current_exit_status(1) ,NULL);
	if (obj_ygg.y->type == AST_COMMAND)
	{
		obj_ygg.y->args = gather_arguments(t, NULL);
		if (!obj_ygg.y->args && get_current_exit_status() != 0)
			return (NULL);
	}
	else if (obj_ygg.y->type == AST_REDIR_IN || obj_ygg.y->type == AST_REDIR_OUT
		|| obj_ygg.y->type == AST_APPEND || obj_ygg.y->type == AST_HEREDOC)
	{
		obj_ygg.y->file = gather_filename(t, e);
		if (!obj_ygg.y->file)
			return (st_prsr_err("syntax error near unexpected token", t->value), \
			set_current_exit_status(2), NULL);
	}
	obj_ygg.left_child_token = find_left_token(t, f);
	obj_ygg.right_child_token = find_right_token(t, e);
	obj_ygg.y->left = make_yggdrasil(obj_ygg.left_child_token, f, t, obj_ygg.y);
	if (get_current_exit_status() != 0 && obj_ygg.y->left == NULL \
		&& obj_ygg.left_child_token && !obj_ygg.left_child_token->used)
		return (NULL);
	obj_ygg.y->right = make_yggdrasil(obj_ygg.right_child_token, t, e, obj_ygg.y);
	if (get_current_exit_status() != 0 && obj_ygg.y->right == NULL \
		&& obj_ygg.right_child_token && !obj_ygg.right_child_token->used)
		return (NULL);
	return (obj_ygg.y);
}
*/

/* Versao 1!!!!
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
	y = new_yggnode(t);
	if (!y)
		return (set_current_exit_status(1) ,NULL);
	if (y->type == AST_COMMAND)
	{
		y->args = gather_arguments(t, NULL);
		if (!y->args && get_current_exit_status() != 0)
			return (NULL);
	}
	else if (y->type == AST_REDIR_IN || y->type == AST_REDIR_OUT
		|| y->type == AST_APPEND || y->type == AST_HEREDOC)
	{
		y->file = gather_filename(t, e);
		if (!y->file)
			return (st_prsr_err("syntax error near unexpected token", t->value), \
			set_current_exit_status(2), NULL);
	}
	left_child_token = find_left_token(t, f);
	right_child_token = find_right_token(t, e);
	y->left = make_yggdrasil(left_child_token, f, t, y);
	if (get_current_exit_status() != 0 && y->left == NULL \
		&& left_child_token && !left_child_token->used)
		return (NULL);
	y->right = make_yggdrasil(right_child_token, t, e, y);
	if (get_current_exit_status() != 0 && y->right == NULL \
		&& right_child_token && !right_child_token->used)
		return (NULL);
	return (y);
}
*/

/* original!!!!!
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
	y = new_yggnode(t);
	if (!y)
	{
		set_current_exit_status(1);
		return (NULL);
	}
	if (y->type == AST_COMMAND)
	{
		y->args = gather_arguments(t, NULL);
		if (!y->args && get_current_exit_status() != 0)
		{
			free(y); // free
			return (NULL);
		}
	}
	else if (y->type == AST_REDIR_IN || y->type == AST_REDIR_OUT
		|| y->type == AST_APPEND || y->type == AST_HEREDOC)
	{
		y->file = gather_filename(t, e);
		if (!y->file)
		{
			st_prsr_err("syntax error near unexpected token", t->value);
			set_current_exit_status(2);
			if (y->args)
				ft_free_strarray(y->args); // free
			free(y); // free
			return (NULL);
		}
	}
	left_child_token = find_left_token(t, f);
	right_child_token = find_right_token(t, e);
	y->left = make_yggdrasil(left_child_token, f, t, y);
	if (get_current_exit_status() != 0 && y->left == NULL \
		&& left_child_token && !left_child_token->used)
	{
		if (y->args)
			ft_free_strarray(y->args); // free
		if (y->file)
			free(y->file); // free
		free(y); // free
		return (NULL);
	}
	y->right = make_yggdrasil(right_child_token, t, e, y);
	if (get_current_exit_status() != 0 && y->right == NULL \
		&& right_child_token && !right_child_token->used)
	{
		if (y->left)
			//free_ast(y->left); // free
		if (y->args)
			ft_free_strarray(y->args); // free
		if (y->file)
			free(y->file); // free
		free(y); // free
		return (NULL);
	}
	return (y);
}
 */