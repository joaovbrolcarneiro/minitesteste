/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* tokenlst_helpers.c                                         :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/* */ /* Updated: 2025/04/17 20:10:00 by hde-barr         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h" // Or relevant header for get_env_value etc.
#include <dirent.h>          // For opendir, readdir, closedir
#include <sys/stat.h>        // For stat, S_ISDIR used in is_valid_exc

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
	if (count < 0)/* Gets environment variable value using allowed functions */
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
		if (eq_ptr) // Check if '=' exists
		{
			key_len = eq_ptr - env[i]; // Calculate length of key part
			if (key_len == var_len && ft_strncmp(env[i], var, key_len) == 0)
				return (eq_ptr + 1); // Return pointer to value part
		}
		i++;
	}
	return (NULL);
}

bool	search_list(char *search, char **env)
{
	char	**list;
	char	**current;
	bool	found;

	if (!search || !env)
		return (false);
	if (is_valid_exc(search))
		return (true);
	list = init_command_list(env);
	if (!list)
		return (false);
	current = list;
	found = false;
	while (*current)
	{
		if (ft_strcmp(search, *current) == 0)
		{
			found = true;
			break ;
		}
		current++;
	}
	ft_free_strarray(list);
	return (found);
}

/* Handles end conditions and returns the final list head */
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

void st_prsr_err(const char *message, const char *token_value)
{
    // Add (char *) casts before passing string literals or const char*
    ft_putstr_fd((char *)"konosubash: ", 2);

    if (message) {
        // Cast const parameter 'message'
        ft_putstr_fd((char *)message, 2);
    } else {
        // Cast string literal
        ft_putstr_fd((char *)"syntax error", 2);
    }

    if (token_value && *token_value)
    {
        // Cast string literal
        ft_putstr_fd((char *)" near unexpected token `", 2);
        // Cast const parameter 'token_value'
        ft_putstr_fd((char *)token_value, 2);
        // Cast string literal
        ft_putstr_fd((char *)"'", 2);
    }

    // Cast string literal
    ft_putstr_fd((char *)"\n", 2);

    // Set the global exit code for syntax errors
    g_exit_code = 2;
}