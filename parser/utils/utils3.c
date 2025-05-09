/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 21:51:16 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:10:49 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
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
