/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenlst_split_input.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:54:43 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:32:38 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

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

	new_token = hb_malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_memset(new_token, 0, sizeof(t_token));
	new_token->value = ft_substr(input, start, token_len);
	if (!new_token->value)
		return (NULL);
	initialize_remaining_token_fields(new_token, input, start, token_len);
	return (new_token);
}

void	initialize_remaining_token_fields(t_token *token,
											const char *input,
											int start,
											int token_len)
{
	int	index_after_token;

	token->id = get_new_token_id();
	token->type = TOKEN_WORD;
	token->coretype = TOKEN_WORD;
	token->rank = RANK_C;
	token->used = false;
	token->err = 0;
	token->literal = false;
	token->next = NULL;
	index_after_token = start + token_len;
	if (token_len == 1 && token->value[0] == '$')
	{
		token->join_next = false;
	}
	else
	{
		token->join_next = (input[index_after_token] != '\0'
				&& !is_whitespace(input[index_after_token])
				&& !ischarset("|<>", input[index_after_token]));
	}
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
