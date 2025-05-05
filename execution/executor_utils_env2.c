/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_env2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 23:44:35 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Helper: Validates and processes a single argument for export */
static int	handle_single_export_arg(char *arg, char ***env)
{
	char	*equal;
	char	*var_name;
	int		update_status;

	equal = ft_strchr(arg, '=');
	var_name = arg;
	if (equal)
		*equal = '\0';
	if (!is_valid_identifier(var_name))
	{
		if (equal)
			*equal = '=';
		ft_putstr_fd("konosubash: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	else if (equal)
	{
		update_status = update_env(env, var_name, equal + 1);
		*equal = '=';
		if (update_status != 0)
			return (1);
	}
	return (0);
}

/* Helper for ft_export: Processes arguments */
static int	process_export_args(char **args, char ***env)
{
	int	i;
	int	overall_exit_code;
	int	arg_status;

	i = 1;
	overall_exit_code = 0;
	while (args[i])
	{
		arg_status = handle_single_export_arg(args[i], env);
		if (arg_status != 0)
			overall_exit_code = arg_status;
		i++;
	}
	return (overall_exit_code);
}

/* Helper for ft_export: Prints sorted env */
static void	print_single_env_var(char *env_var)
{
	char	*equal_sign;

	equal_sign = ft_strchr(env_var, '=');
	write(STDOUT_FILENO, "declare -x ", 11);
	if (equal_sign)
	{
		write(STDOUT_FILENO, env_var, equal_sign - env_var);
		write(STDOUT_FILENO, "=\"", 2);
		ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
		write(STDOUT_FILENO, "\"", 1);
	}
	else
	{
		ft_putstr_fd(env_var, STDOUT_FILENO);
	}
	write(STDOUT_FILENO, "\n", 1);
}

/**
 * @brief Sorts and prints environment variables in 'declare -x' format.
 * Skips the special '_' variable.
 * @param env Pointer to the environment array (char ***).
 * @return int 0 on success, 1 on error (e.g., allocation failure).
 */
static int	print_sorted_env(char ***env)
{
	char	**sorted;
	int		i;

	sorted = sort_env(*env);
	if (!sorted)
		return (perror("minishell: export/sort_env"), 1);
	i = 0;
	while (sorted[i])
	{
		if (!(ft_strncmp(sorted[i], "_=", 2) == 0
				|| ft_strcmp(sorted[i], "_") == 0))
		{
			print_single_env_var(sorted[i]);
		}
		i++;
	}
	free(sorted);
	return (0);
}

/* Builtin export command */
int	ft_export(char **args, char ***env)
{
	if (!args[1])
		return (print_sorted_env(env));
	else
		return (process_export_args(args, env));
}
