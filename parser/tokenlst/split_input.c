/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:53:41 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 15:31:39 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

bool	proximity_exception(char *input, int i)
{
	if ((!ischarset("|<>", input[i]) && input[i] != ' ') && \
	input[i] && !ischarset("|<>", *input))
		return (true);
	return (false);
}

void	perform_quote_concatenation(t_token *token_list)
{
	t_token	*currnt;
	t_token	*node_to_rm;
	char	*joined_value;
	char	*temp_value_ptr;

	node_to_rm = NULL;
	joined_value = NULL;
	temp_value_ptr = NULL;
	currnt = token_list;
	perform_quote_con_part2(currnt, node_to_rm, joined_value, temp_value_ptr);
}

void	st_prsr_err(const char *message, const char *token_value)
{
	ft_putstr_fd((char *)"konosubash: ", 2);
	if (message)
		ft_putstr_fd((char *)message, 2);
	else
		ft_putstr_fd((char *)"syntax error", 2);
	if (token_value && *token_value)
	{
		ft_putstr_fd((char *)" near unexpected token `", 2);
		ft_putstr_fd((char *)token_value, 2);
		ft_putstr_fd((char *)"'", 2);
	}
	ft_putstr_fd((char *)"\n", 2);
	set_current_exit_status(2);
}

int	get_token_len(char *input)
{
	int		i;
	char	start_char;

	i = 1;
	start_char = *input;
	if (ischarset("\"'", start_char))
	{
		while (input[i] && input[i] != start_char)
			i++;
		if (input[i] == start_char)
			i++;
		else
			i = 1;
	}
	else
		i = get_unquoted_token_len(input);
	return (i);
}

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' \
	|| c == '\v' || c == '\f' || c == '\r');
}
