/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 21:51:16 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 17:02:05 by hde-barr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "minishell_part2.h"

bool	is_executable(const char *path)
{
	if (access(path, F_OK | X_OK) == 0)
		return (true);
	return (false);
}

bool	is_regular_file(const char *path)
{
	struct stat	path_stat;

	if (stat(path, &path_stat) != 0)
	{
		return (false);
	}
	return (S_ISREG(path_stat.st_mode));
}

bool	is_valid_exc(const char *path)
{
	return (is_regular_file(path) && is_executable(path));
}

void	is_minishell_exit(char *input)
{
	char	*res;

	res = ft_strtrim(input, " ");
	if (!res)
		return ;
	if (ft_strcmp("exit", res) == 0)
		exit(0);
}

bool	has_parser_error(t_token *token)
{
	while (token)
	{
		if (token->err)
			return (true);
		token = token->next;
	}
	return (false);
}
