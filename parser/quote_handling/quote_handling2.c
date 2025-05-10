/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handling2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 19:19:01 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:51:17 by jbrol-ca         ###   ########.fr       */
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

char	*handle_single_quotes(t_token *token, char *original_value)
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

char	*handle_double_quotes(t_token *token, char **env, \
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

char	*handle_dollar_single_quotes(t_token *token, char *original_value)
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
