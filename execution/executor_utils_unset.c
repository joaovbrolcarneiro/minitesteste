/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_unset.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 20:33:05 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Helper for ft_unset: Finds and removes one var */
static int	find_and_remove_var(t_shell *shell, const char *varname)
{
	int		j;
	int		k;
	size_t	name_len;

	j = 0;
	name_len = ft_strlen(varname);
	while (shell->env[j])
	{
		if (ft_strncmp(shell->env[j], varname, name_len) == 0 \
			&& shell->env[j][name_len] == '=')
		{
			k = j;
			while (shell->env[k + 1])
			{
				shell->env[k] = shell->env[k + 1];
				k++;
			}
			shell->env[k] = NULL;
			return (1);
		}
		j++;
	}
	return (0);
}

/* Builtin unset command */
int	ft_unset(char **args, t_shell *shell)
{
	int	i;
	int	exit_code;

	i = 1;
	exit_code = 0;
	if (!args[1])
		return (0);
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("konosubash: unset: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			exit_code = 1;
		}
		else
			find_and_remove_var(shell, args[i]);
		i++;
	}
	return (exit_code);
}

char	*ft_strcat(char *dest, const char *src)
{
	char	*tmp;

	tmp = dest;
	while (*dest)
	{
		dest++;
	}
	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (tmp);
}

int	ensure_exported_no_value(char ***env, char *var_name)
{
	int		i;
	size_t	var_len;
	char	*new_entry;

	i = 0;
	var_len = ft_strlen(var_name);
	while ((*env)[i])
	{
		if (ft_strncmp((*env)[i], var_name, var_len) == 0
			&& ((*env)[i][var_len] == '=' || (*env)[i][var_len] == '\0'))
		{
			return (0);
		}
		i++;
	}
	new_entry = ft_strdup(var_name);
	if (!new_entry)
	{
		perror("minishell: ensure_exported_no_value: ft_strdup");
		return (1);
	}
	if (add_new_env_var(env, new_entry) != 0)
		return (1);
	return (0);
}
