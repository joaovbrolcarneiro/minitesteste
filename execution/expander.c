/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 18:21:19 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Gets value from environment, returns "" if not found */
char	*get_env_value_exp(const char *name, char **env)
{
	char	*val;

	val = get_env_value(env, name);
	if (!val)
		return (""); // Return empty literal, not allocated
	return (val); // Return pointer into existing env
}

/* Checks if char c is valid at position pos in a variable name */
int	is_valid_var_char(char c, int pos)
{
	if (pos == 0)
	{
		return ((c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| c == '_');
	}
	return ((c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9')
		|| c == '_');
}

/* Helper loop for expand_variables */
/* Returns 0 on success, 1 on failure */
static int	expansion_loop(t_exp_vars *v)
{
	while (v->input[v->i])
	{
		if (v->input[v->i] == '$')
		{
			// handle_dollar_expansion uses helpers which might call
			// append_char (now assumed hb_malloc based) or ft_itoa/ft_substr (GC'd)
			if (handle_dollar_expansion(v) != 0)
				return (1);
		}
		else
		{
			// append_normal_char calls append_char (now assumed hb_malloc based)
			if (append_normal_char(v) != 0)
				return (1);
		}
	}
	return (0);
}

/* Expands variables ($VAR, $?) in a single string */
/* Returns GC-managed string or NULL on error */
char	*expand_variables(const char *input, char **env)
{
	t_exp_vars	v;
	int			loop_status;

	if (!input)
		return (NULL);
	// Assumes exp_var_init uses hb_malloc for v.result buffer
	if (exp_var_init(&v, input, env) != 0)
		return (NULL);
	loop_status = expansion_loop(&v);
	if (loop_status != 0)
		// return (free(v.result), NULL); // REMOVED: v.result is GC-managed
		return (NULL); // Error occurred, GC will handle buffer
	// Assumes append_char uses hb_malloc based realloc
	if (!append_char(&v.result, &v.res_len, &v.res_cap, '\0'))
		// return (free(v.result), perror("m: expand null term"), NULL); // REMOVED
		return (perror("minishell: expand null term"), NULL); // Error, GC handles buffer
	return (v.result); // Return GC-managed string
}

/* Applies expand_variables to relevant tokens in a list */
/* Assumes expand_variables returns GC-managed memory */
void	expand_token_list(t_token *token_list, char **env)
{
	t_token	*cur;
	char	*expanded;
	// char	*original_value; // No longer needed

	cur = token_list;
	while (cur)
	{
		if (cur->value && !cur->literal)
		{
			// original_value = cur->value; // Store original pointer
			// expand_variables returns GC-managed string or NULL
			expanded = expand_variables(cur->value, env);

			if (expanded == NULL && cur->value != NULL)
			{
				// Expansion failed, keep original value? Or set to empty?
				// For now, just report warning, keep original GC'd value.
				ft_putstr_fd("minishell: warning: expansion failed\n", 2);
			}
			// Check if expansion returned a different pointer (even if content is same)
			// or if original value was non-NULL and expansion result is NULL (error case handled above)
			else if (expanded != cur->value)
			{
				// free(original_value); // REMOVED: original_value is GC-managed
				cur->value = expanded; // Assign the new GC-managed pointer
			}
			// else: expanded == cur->value (no change) or both NULL. Do nothing.
		}
		cur = cur->next;
	}
}