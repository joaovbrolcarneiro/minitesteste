/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenlst_split_input.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:54:43 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 15:46:35 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int	get_unquoted_token_len(char *input)
{
	int		i;
	char	start_char;

	i = ft_strsetlen(input, "\"' |<>");
	start_char = *input;
	if (i == 0 && ischarset("|<>", start_char))
	{
		i = 1;
		if (ischarset("<>", start_char) && input[1] == start_char)
			i = 2;
	}
	else if (i == 0 && *input != '\0')
		i = 1;
	else if (i == 0 && *input == '\0')
		return (0);
	if (i == 0)
		i = 1;
	return (i);
}

static void	add_token_to_list(t_token **first, t_token **lst,
	t_token *new_token)
{
	if (!*first)
	{
		*first = new_token;
		*lst = *first;
	}
	else
	{
		(*lst)->next = new_token;
		*lst = new_token;
	}
}

t_token	*create_and_init_token(char *input, int start, int token_len)
{
	t_token	*new_token;
	int		index_after_token;

	index_after_token = start + token_len;
	new_token = hb_malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_memset(new_token, 0, sizeof(t_token));
	new_token->value = ft_substr(input, start, token_len);
	if (!new_token->value)
		return (NULL);
	new_token->id = get_new_token_id();
	new_token->type = TOKEN_WORD;
	new_token->coretype = TOKEN_WORD;
	new_token->rank = RANK_C;
	new_token->used = false;
	new_token->err = 0;
	new_token->literal = false;
	new_token->next = NULL;
	new_token->join_next = (input[index_after_token] != '\0' \
	&& !is_whitespace(input[index_after_token]) \
	&& !ischarset("|<>", input[index_after_token]));
	return (new_token);
}

static int	process_single_token_block(char *input, int *start,
	t_token **first, t_token **lst)
{
	int		token_len;
	t_token	*new_token;

	while (input[*start] && input[*start] == ' ')
		(*start)++;
	if (!input[*start])
		return (1);
	token_len = get_token_len(input + *start);
	if (token_len <= 0)
	{
		ft_putstr_fd("konosubash: split_input: Error or zero token len\n", 2);
		return (-1);
	}
	new_token = create_and_init_token(input, *start, token_len);
	if (!new_token)
		return (-1);
	add_token_to_list(first, lst, new_token);
	(*start) += token_len;
	return (0);
}

t_token	*split_input(char *input, int unused_i)
{
	t_token	*first;
	t_token	*lst;
	int		start;
	int		status;

	(void)unused_i;
	first = NULL;
	lst = NULL;
	start = 0;
	while (input[start])
	{
		status = process_single_token_block(input, &start, &first, &lst);
		if (status == -1)
		{
			set_current_exit_status(1);
			return (NULL);
		}
		if (status == 1)
			break ;
	}
	return (first);
}
