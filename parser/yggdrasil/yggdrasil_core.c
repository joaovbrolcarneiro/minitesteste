/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 17:22:43 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

t_node_tree	*mke_yggdrasil(t_token *t, t_token *f, t_token *e, t_node_tree *y)
{
	t_obj_ygg	obj;

	(void)y;
	obj = make_yggdrasil_init();
	if (!t || t == e || t->used)
		return (NULL);
	obj.y = new_yggnode(t);
	if (!obj.y)
		return (set_current_exit_status(1), NULL);
	if (obj.y->type == AST_COMMAND)
	{
		obj.y->args = gather_arguments(t, NULL);
		if (!obj.y->args && get_current_exit_status() != 0)
			return (NULL);
	}
	else if (is_redirection(obj))
	{
		obj.y->file = gather_filename(t, e);
		if (!obj.y->file)
			return (handle_parser_error(t), NULL);
	}
	if (set_ygg_children(&obj, t, f, e))
		return (NULL);
	return (obj.y);
}

t_node_tree	*new_yggnode(t_token *token)
{
	t_node_tree	*new_node;

	if (!token)
		return (NULL);
	if (token->used == true)
		return (NULL);
	new_node = hb_malloc(sizeof(t_node_tree));
	if (!new_node)
	{
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
	new_node->err = token->err;
	new_node->coretype = token->coretype;
	new_node->literal = token->literal;
	return (new_node);
}



/**
* Sets the left and right children of a new AST node (obj->y) based on token t.
* Validates pipe operands and checks for errors during child creation.
* Returns true if an error occurs during child setting, false otherwise.
*/
static int	check_child_node_errors(t_node_tree *child_node,
		t_obj_ygg *parent_obj_ptr, t_token *child_token_val,
		int is_left_child_flag)
{
	bool	has_operand_token_check;
	bool	general_error_condition;
	bool	pipe_specific_error_condition;

	if (is_left_child_flag)
		has_operand_token_check = has_left(*parent_obj_ptr);
	else
		has_operand_token_check = has_right(*parent_obj_ptr);
	general_error_condition = (get_current_exit_status() != 0
			&& child_node == NULL && has_operand_token_check);
	pipe_specific_error_condition = (parent_obj_ptr->y->type == AST_PIPE
			&& child_node == NULL && child_token_val != NULL);
	if (general_error_condition || pipe_specific_error_condition)
		return (1);
	return (0);
}

/**
 * Sets the left and right children of a new AST node (obj->y) based on token t.
 * Validates pipe operands and checks for errors during child creation.
 * Returns true if an error occurs during child setting, false otherwise.
 */
bool	set_ygg_children(t_obj_ygg *obj, t_token *t, t_token *f, t_token *e)
{
	obj->left_child_token = find_left_token(t, f);
	obj->right_child_token = find_right_token(t, e);
	if (obj->y->type == AST_PIPE)
	{
		if (obj->left_child_token == NULL || obj->right_child_token == NULL)
		{
			handle_parser_error(t);
			return (true);
		}
	}
	obj->y->left = mke_yggdrasil(obj->left_child_token, f, t, obj->y);
	if (check_child_node_errors(obj->y->left, obj,
			obj->left_child_token, 1) != 0)
		return (true);
	obj->y->right = mke_yggdrasil(obj->right_child_token, t, e, obj->y);
	if (check_child_node_errors(obj->y->right, obj,
			obj->right_child_token, 0) != 0)
		return (true);
	return (false);
}

t_obj_ygg	make_yggdrasil_init(void)
{
	t_obj_ygg	obj;

	obj.y = NULL;
	obj.left_child_token = NULL;
	obj.right_child_token = NULL;
	return (obj);
}
