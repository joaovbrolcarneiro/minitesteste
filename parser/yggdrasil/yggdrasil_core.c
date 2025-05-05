/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yggdrasil_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:25:45 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 22:55:06 by jbrol-ca         ###   ########.fr       */
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

bool	set_ygg_children(t_obj_ygg *obj, t_token *t, t_token *f, t_token *e)
{
	bool	left_error_condition;
	bool	right_error_condition;

	obj->left_child_token = find_left_token(t, f);
	obj->right_child_token = find_right_token(t, e);
	obj->y->left = mke_yggdrasil(obj->left_child_token, f, t, obj->y);
	left_error_condition = (get_current_exit_status() != 0
			&& obj->y->left == NULL
			&& has_left(*obj));
	if (left_error_condition)
		return (true);
	obj->y->right = mke_yggdrasil(obj->right_child_token, t, e, obj->y);
	right_error_condition = (get_current_exit_status() != 0
			&& obj->y->right == NULL
			&& has_right(*obj));
	if (right_error_condition)
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

bool	is_redirection(t_obj_ygg obj)
{
	return (obj.y->type == AST_REDIR_IN
		|| obj.y->type == AST_REDIR_OUT
		|| obj.y->type == AST_APPEND
		|| obj.y->type == AST_HEREDOC);
}
