/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_lst_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:14:05 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 15:29:54 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h" 
#include <dirent.h>          
#include <sys/stat.h>        

char	**get_path_list(char **env)
{
	char	*path;
	char	**path_list;

	path = get_envar(env, "PATH");
	if (!path)
		return (NULL);
	path_list = ft_split(path, ':');
	return (path_list);
}

char	**command_list_malloc(char **env)
{
	long long	count;
	char		**allocated_list;

	allocated_list = NULL;
	count = count_commands_in_path(env);
	if (count < 0)
		return (NULL);
	allocated_list = malloc(sizeof(char *) * (count + 1));
	if (!allocated_list)
		perror("minishell: malloc command list");
	return (allocated_list);
}

char	*get_envar(char **env, char *var)
{
	int		i;
	size_t	var_len;
	char	*eq_ptr;
	size_t	key_len;

	i = 0;
	if (!env || !var)
		return (NULL);
	var_len = ft_strlen(var);
	while (env[i])
	{
		eq_ptr = ft_strchr(env[i], '=');
		if (eq_ptr)
		{
			key_len = eq_ptr - env[i];
			if (key_len == var_len && ft_strncmp(env[i], var, key_len) == 0)
				return (eq_ptr + 1);
		}
		i++;
	}
	return (NULL);
}

bool	search_list(char *search, char **env)
{
	char	**list;
	char	**current;

	if (!search || !*search || !env || ft_strcmp(search, ".") == 0
		|| ft_strcmp(search, "..") == 0)
		return (false);
	if (ft_strchr(search, '/') != NULL)
		return (is_valid_exc(search));
	else
	{
		list = init_command_list(env);
		if (!list)
			return (false);
		current = list;
		while (*current)
		{
			if (ft_strcmp(search, *current) == 0)
				return (ft_free_strarray(list), true);
			current++;
		}
		return (ft_free_strarray(list), false);
	}
}

t_token	*finalize_list(t_token *first, t_token *last)
{
	if (last)
		last->next = NULL;
	if (!first || !first->value)
	{
		if (first)
			free(first);
		return (NULL);
	}
	return (first);
}
