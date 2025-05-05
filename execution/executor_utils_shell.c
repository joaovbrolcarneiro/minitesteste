/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_shell.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 22:53:16 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Duplicates a string array */
/* Caller must free using ft_free_strarray */
char	**ft_strdup_array(char **array)
{
	int		i;
	int		len;
	char	**dup;

	len = 0;
	if (!array)
		return (NULL);
	while (array[len])
		len++;
	dup = (char **)hb_malloc(sizeof(char *) * (len + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = ft_strdup(array[i]);
		if (!dup[i])
		{
			return (NULL);
		}
		i++;
	}
	dup[len] = NULL;
	return (dup);
}

/* Frees a string array */
void	ft_free_strarray(char **array)
{
	if (!array)
		return ;
	free(array);
}

/* Initializes the shell structure */
void	init_shell(t_shell *shell, char **env)
{
	if (!shell)
		return ;
	shell->env = ft_strdup_array(env);
	shell->saved_stdin = dup(STDIN_FILENO);
	shell->saved_stdout = dup(STDOUT_FILENO);
	if (shell->saved_stdin == -1 || shell->saved_stdout == -1 || !shell->env)
	{
		perror("minishell: init_shell resource allocation failed");
		if (shell->saved_stdin != -1)
			close(shell->saved_stdin);
		if (shell->saved_stdout != -1)
			close(shell->saved_stdout);
		exit(EXIT_FAILURE);
	}
	shell->heredoc_fd = -1;
	shell->in_heredoc = 0;
	shell->ast_root = NULL;
}

/* Cleans up shell resources */
void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env)
	{
		shell->env = NULL;
	}
	if (shell->saved_stdin >= 0)
	{
		close(shell->saved_stdin);
		shell->saved_stdin = -1;
	}
	if (shell->saved_stdout >= 0)
	{
		close(shell->saved_stdout);
		shell->saved_stdout = -1;
	}
	get_next_line(GNL_CLEANUP);
	minigarbege_colector();
}

int	ft_echo(char **args)
{
	int		i;
	bool	print_newline;

	i = 1;
	print_newline = true;
	while (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		print_newline = false;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (print_newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}
