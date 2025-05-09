/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:31:11 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Helper: Appends null-terminated string content to v->result */
int	append_str_to_exp_vars(t_exp_vars *v, char *str)
{
	size_t	k;

	k = 0;
	if (!str)
		return (0);
	while (str[k])
	{
		if (!append_char(&v->result, &v->res_len, &v->res_cap, str[k]))
			return (1);
		k++;
	}
	return (0);
}

/* Helper: Extracts variable name length */
static size_t	get_var_name_len(const char *input, size_t start_index)
{
	size_t	current_pos;
	int		char_pos;

	current_pos = start_index;
	char_pos = 0;
	while (input[current_pos] && \
		is_valid_var_char(input[current_pos], char_pos))
	{
		current_pos++;
		char_pos++;
	}
	return (current_pos - start_index);
}

/* Appends expanded variable ($VAR) to result */
/* Assumes append_str_to_exp_vars handles errors */
void	handle_variable(t_exp_vars *v) // free retirado
{
	char	*var_name;
	size_t	var_len;

	v->var_start = v->i;
	var_len = get_var_name_len(v->input, v->var_start);
	if (var_len == 0)
		return ;
	var_name = ft_substr(v->input, v->var_start, var_len);
	v->i += var_len;
	if (!var_name)
	{
		perror("minishell: ft_substr");
		return ;
	}
	v->var_value = get_env_value_exp(var_name, v->env);
	append_str_to_exp_vars(v, v->var_value);
}

int	handle_dollar_brace_expansion(t_exp_vars *v)
{
	(v->i)++;
	if (is_valid_var_char(v->input[v->i], 0))
	{
		handle_variable(v);
		if (v->input[v->i] == '}')
			(v->i)++;
	}
	else
	{
		if (!append_char(&v->result, &v->res_len, &v->res_cap, '$'))
			return (1);
		if (!append_char(&v->result, &v->res_len, &v->res_cap, '{'))
			return (1);
	}
	return (0);
}

int	handle_dollar_expansion(t_exp_vars *v)
{
	(v->i)++;
	if (v->input[v->i] == '\0')
	{
		if (!append_char(&v->result, &v->res_len, &v->res_cap, '$'))
			return (1);
		return (0);
	}
	if (v->input[v->i] == '?')
		handle_exit_status(v);
	else if (v->input[v->i] == '{')
	{
		if (handle_dollar_brace_expansion(v) != 0)
			return (1);
	}
	else if (is_valid_var_char(v->input[v->i], 0))
		handle_variable(v);
	else
	{
		if (!append_char(&v->result, &v->res_len, &v->res_cap, '$'))
			return (1);
	}
	return (0);
}