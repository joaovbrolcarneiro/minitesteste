/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 19:19:01 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:54:09 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

// New static helper for $"...", norminette compliant
char	*handle_dollar_double_quotes(t_token *token, char **env, \
										char *original_value)
{
	char	*content;
	char	*expanded_content;
	size_t	len;

	token->literal = false;
	len = ft_strlen(original_value);
	if (len < 3 || original_value[0] != '$' || original_value[1] != '"' \
		|| original_value[len - 1] != '"')
		return (ft_strdup(original_value));
	content = ft_substr(original_value, 2, len - 3);
	if (!content)
	{
		perror("konosubash: ft_substr $\"");
		return (NULL);
	}
	expanded_content = expand_variables(content, env);
	if (!expanded_content)
	{
		perror("konosubash: expand_variables for $\"");
		return (ft_strdup(""));
	}
	return (expanded_content);
}

char	*process_regular_quotes(t_quote_params *params,
									char *current_value)
{
	char	quote_char;

	quote_char = current_value[0];
	is_quote_opened(current_value, params->is_unclosed_flag);
	if (*(params->is_unclosed_flag))
		return (current_value);
	if (quote_char == '\'')
		return (handle_single_quotes(params->token, current_value));
	else
		return (handle_double_quotes(params->token, params->env,
				current_value, params->is_literal_operator));
}

char	*determine_and_process_quotes(t_quote_params *params,
											char *current_value)
{
	if (current_value[0] == '$' && ft_strlen(current_value) > 1
		&& current_value[1] == '\'')
		return (handle_dollar_single_quotes(params->token, current_value));
	else if (current_value[0] == '$' && ft_strlen(current_value) > 1
		&& current_value[1] == '"')
		return (handle_dollar_double_quotes(params->token, params->env,
				current_value));
	else if (current_value[0] == '\'' || current_value[0] == '"')
		return (process_regular_quotes(params, current_value));
	else
	{
		params->token->literal = params->is_literal_operator;
		return (current_value);
	}
}

char	*finalize_token_assignment(t_token *token, char *new_val,
										char *orig_val, int *is_unclosed_flag)
{
	if (*is_unclosed_flag || new_val == NULL)
	{
		token->err = 1;
		token->value = ft_strdup("");
		if (!token->value)
			return (NULL);
		return (token->value);
	}
	if (new_val != orig_val)
		free(orig_val);
	token->value = new_val;
	return (token->value);
}

char	*quote_handler(t_token *token, char **env, int *is_unclosed_flag)
{
	char			*original_value;
	char			*new_value;
	t_quote_params	params;

	*(is_unclosed_flag) = 0;
	if (!token || !token->value)
		return (NULL);
	original_value = token->value;
	params.token = token;
	params.env = env;
	params.is_unclosed_flag = is_unclosed_flag;
	params.is_literal_operator = handler_quote_operator(original_value);
	new_value = determine_and_process_quotes(&params, original_value);
	return (finalize_token_assignment(token, new_value, original_value,
			is_unclosed_flag));
}
