/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 19:19:01 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:46:49 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

/* Checks for unclosed quotes */
char	*is_quote_opened(char *input, int *is_unclosed)
{
	size_t	len;
	char	start_quote;

	*is_unclosed = 0;
	if (!input)
		return (NULL);
	len = ft_strlen(input);
	if (ischarset("\"'", input[0]))
	{
		start_quote = input[0];
		if (len == 1 || input[0] != input[len - 1])
		{
			ft_putstr_fd(RED "konosubash: parser error: unclosed quote `", 2);
			ft_putstr_fd(input, 2);
			ft_putstr_fd("'\n" RESET, 2);
			*is_unclosed = 1;
		}
	}
	return (input);
}

bool	handler_quote_operator(char *input)
{
	int			i;
	const char	*dictionary[] = {
		"\"<<\"", "\">>\"", "\"<\"", "\">\"",
		"'<<'", "'>>'", "'<'", "'>'",
		"\"|\"", "'|'", NULL};

	i = 0;
	if (!input)
		return (false);
	while (dictionary[i])
	{
		if (ft_strcmp(dictionary[i], input) == 0)
			return (true);
		i++;
	}
	return (false);
}

static char	*handle_single_quotes(t_token *token, char *original_value)
{
	char	*trimmed_value;

	token->literal = true;
	trimmed_value = ft_strtrim(original_value, "'");
	if (!trimmed_value)
	{
		return (original_value);
	}
	token->value = trimmed_value;
	return (token->value);
}

static char	*handle_double_quotes(t_token *token, char **env, \
char *original_value, bool is_operator_literal)
{
	char	*trimmed_value;
	char	*expanded_value;
	int		cmd_len;

	token->literal = is_operator_literal;
	trimmed_value = ft_strtrim(original_value, "\"");
	if (!trimmed_value)
	{
		token->value = NULL;
		return (NULL);
	}
	token->value = trimmed_value;
	if (!token->literal)
	{
		cmd_len = quote_handler_counter(trimmed_value, env);
		expanded_value = quote_handler_cpy(cmd_len, trimmed_value, env);
		if (!expanded_value)
		{
			token->value = NULL;
			return (NULL);
		}
		token->value = expanded_value;
	}
	return (token->value);
}

static char	*handle_dollar_single_quotes(t_token *token, char *original_value)
{
	char	*content;
	size_t	len;

	token->literal = true;
	len = ft_strlen(original_value);
	if (len < 3 || original_value[0] != '$' || original_value[1] != '\'' \
		|| original_value[len - 1] != '\'')
	{
		return (ft_strdup(original_value));
	}
	content = ft_substr(original_value, 2, len - 3);
	if (!content)
		perror("konosubash: ft_substr $'");
	return (content);
}

// New static helper for $"...", norminette compliant
static char	*handle_dollar_double_quotes(t_token *token, char **env, \
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

static char	*process_regular_quotes(t_quote_params *params,
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

static char	*determine_and_process_quotes(t_quote_params *params,
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

static char	*finalize_token_assignment(t_token *token, char *new_val,
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