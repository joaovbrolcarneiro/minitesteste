/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_shell.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 16:02:51 by jbrol-ca         ###   ########.fr       */
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
	// Use hb_malloc for the array pointer itself
	dup = (char **)hb_malloc(sizeof(char *) * (len + 1));
	if (!dup)
		return (NULL); // Allocation failed
	i = 0;
	while (i < len)
	{
		// Use ft_strdup (hb_malloc) for the strings
		dup[i] = ft_strdup(array[i]);
		if (!dup[i])
		{
			// return (ft_free_strarray(dup), NULL); // REMOVED: GC handles partial array/strings
			return (NULL); // Error, GC will clean up dup and dup[0..i-1]
		}
		i++;
	}
	dup[len] = NULL;
	return (dup); // Return GC-managed array pointer
}

/* Frees a string array */
void ft_free_strarray(char **array)
{
    if (!array)
        return ;
    // --- DEBUG ---
    dprintf(2, "[%d] DEBUG: ft_free_strarray freeing pointer %p\n", getpid(), (void *)array);
    // --- END DEBUG ---
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
		if (shell->env)
			//ft_free_strarray(shell->env);
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
		//ft_free_strarray(shell->env);
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
