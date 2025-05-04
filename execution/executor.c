/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 19:29:38 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Part of execute_external_command: child process logic */
static void	child_exec_external(t_shell *shell, char **args)
{
	char	*cmd_path;

	cmd_path = NULL;
	handle_child_signals(); // Set signals for child

	// Determine command path
	if (args[0] && ft_strchr(args[0], '/'))
	{ // Added braces
		cmd_path = ft_strdup(args[0]); // Assumes ft_strdup uses hb_malloc (GC'd)
	} // Added braces
	else if (args[0])
	{ // Added braces
		cmd_path = find_command_path(args[0], shell->env); // Returns GC'd path
	} // Added braces

	// *** DEBUG PRINT ADDED HERE ***
	if (cmd_path)
		dprintf(2, "DEBUG: Found path: [%s]\n", cmd_path);
	else
		dprintf(2, "DEBUG: find_command_path returned NULL for [%s]\n", args[0]);
	// *** END DEBUG PRINT ***

	// Handle command not found
	if (!cmd_path)
	{
		ft_putstr_fd("konosubash: command not found: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd("\n", 2);
		exit(127); // Standard exit code for command not found
	}

	// Execute the command
	execve(cmd_path, args, shell->env);

	// execve only returns on error
	perror("konosubash: execve");
	// free(cmd_path); // Correctly removed previously

	// Determine exit code based on errno
	if (errno == EACCES)
		exit(126); // Standard exit code for permission denied
	exit(EXIT_FAILURE); // Default exit for other execve errors
}

/* Part of execute_external_command: parent process logic */
static int	parent_wait_external(pid_t pid)
{
	int	wait_status;
	int	exit_code;

	exit_code = EXIT_FAILURE;
	handle_parent_signals();
	waitpid(pid, &wait_status, 0);
	if (WIFEXITED(wait_status))
		exit_code = WEXITSTATUS(wait_status);
	else if (WIFSIGNALED(wait_status))
	{
		exit_code = 128 + WTERMSIG(wait_status);
		if (WTERMSIG(wait_status) == SIGQUIT)
			ft_putstr_fd("Quit (core dumped)\n", 2);
		else if (WTERMSIG(wait_status) == SIGINT)
			ft_putstr_fd("\n", 2);
	}
	return (exit_code);
}

/* Executes external command */
int	execute_external_command(t_shell *shell, char **args)
{
    pid_t	pid;

    dprintf(2, "DEBUG: execute_external_command called with args[0]: [%s]\n",
            (args && args[0]) ? args[0] : "NULL"); // DEBUG

    if (!args || !args[0] || !args[0][0])
        return (1);

    pid = fork();
    if (pid == 0)
    {
        child_exec_external(shell, args); // Contains the cmd_path debug print
    }
    else if (pid < 0)
    {
        return (perror("minishell: fork"), 1);
    }
    else if (pid > 0)
    {
        return (parent_wait_external(pid));
    }
    return (EXIT_FAILURE);
}

/* Checks and executes builtins, returns status or -1 if not a builtin */
static int	check_and_run_builtin(t_shell *shell, char **args)
{
	if (ft_strcmp(args[0], "cd") == 0)
		return (ft_cd(args, &shell->env));
	if (ft_strcmp(args[0], "echo") == 0)
		return (ft_echo(args));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (ft_pwd());
	if (ft_strcmp(args[0], "exit") == 0)
		return (ft_exit(args, shell));
	if (ft_strcmp(args[0], "export") == 0)
		return (ft_export(args, &shell->env));
	if (ft_strcmp(args[0], "unset") == 0)
		return (ft_unset(args, shell));
	return (-1);
}

/* Dispatches command execution (builtin or external) */
int	execute_command(t_shell *shell, char **args)
{
    int	builtin_status;

    dprintf(2, "DEBUG: execute_command called with args[0]: [%s]\n",
            (args && args[0]) ? args[0] : "NULL"); // DEBUG

    if (!args || !args[0] || !args[0][0])
        return (0);
    builtin_status = check_and_run_builtin(shell, args);
    if (builtin_status != -1)
    {
        dprintf(2, "DEBUG: execute_command running as builtin, status: %d\n", builtin_status); // DEBUG
        return (builtin_status);
    }
    dprintf(2, "DEBUG: execute_command running as external\n"); // DEBUG
    return (execute_external_command(shell, args));
}
