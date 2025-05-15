/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 23:04:19 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

int handle_heredoc_parent_logic(pid_t pid, int pipefd[2])
{
    int child_process_status;
    int wait_ret;

    close(pipefd[1]); // Parent closes the write end of the pipe

    // Loop to handle EINTR for waitpid (interruption by a signal)
    do {
        wait_ret = waitpid(pid, &child_process_status, 0);
    } while (wait_ret == -1 && errno == EINTR);

    if (wait_ret == -1) // waitpid failed for a reason other than EINTR
    {
        perror("minishell: waitpid failed in heredoc_parent_logic");
        close(pipefd[0]); // Close the read end of the pipe
        set_current_exit_status(1); // General error
        return (1); // Indicate failure
    }

    // Check if child exited normally and successfully
    if (WIFEXITED(child_process_status))
    {
        if (WEXITSTATUS(child_process_status) != EXIT_SUCCESS)
        {
            // Child exited with a non-zero status
            close(pipefd[0]);
            set_current_exit_status(WEXITSTATUS(child_process_status));
            // ft_putstr_fd("DEBUG: Heredoc child exited with error.\n", STDERR_FILENO);
            return (1); // Indicate failure
        }
        // Child exited successfully (status 0)
    }
    else if (WIFSIGNALED(child_process_status))
    {
        // Child was terminated by a signal (e.g., Ctrl+C if not handled by child)
        close(pipefd[0]);
        // ft_putstr_fd("DEBUG: Heredoc child terminated by signal.\n", STDERR_FILENO);
        // Bash typically exits with 128 + signal number
        if (WTERMSIG(child_process_status) == SIGINT)
        {
            ft_putstr_fd("\n", STDOUT_FILENO); // Newline after ^C
            set_current_exit_status(130);
        }
        else
        {
            set_current_exit_status(128 + WTERMSIG(child_process_status));
        }
        return (1); // Indicate failure (as heredoc was interrupted)
    }
    else
    {
        // Unknown status for child process (should not happen with standard waitpid)
        close(pipefd[0]);
        // ft_putstr_fd("DEBUG: Heredoc child unknown termination.\n", STDERR_FILENO);
        set_current_exit_status(1); // Generic error
        return (1); // Indicate failure
    }

    // If we reach here, child exited successfully with status 0
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        perror("minishell: handle_heredoc_parent_logic: dup2 stdin failed");
        close(pipefd[0]);
        return (1); // Indicate failure
    }
    close(pipefd[0]); // Close the original pipe fd, STDIN_FILENO now has it
    return (0); // Success
}


int	handle_heredoc(t_node_tree *node, t_shell *shell)
{
	int		pipefd[2];
	char	*delimiter;
	pid_t	pid;

	delimiter = heredoc_init_and_get_delimiter(node, shell);
	if (!delimiter)
		return (1);
	if (pipe(pipefd) == -1)
	{
		perror("konosubash: handle_heredoc: pipe failed");
		return (1);
	}
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		perror("konosubash: handle_heredoc: fork failed");
		return (1);
	}
	if (pid == 0)
	{
		execute_heredoc_child(pipefd[1], pipefd[0], delimiter, shell);
	}
	return (handle_heredoc_parent_logic(pid, pipefd));
}

int	handle_heredoc_eof(const char *delimiter)
{
	ft_putstr_fd("konosubash: warning: heredoc EOF (wanted `", 2);
	ft_putstr_fd((char *)delimiter, 2);
	ft_putstr_fd("')\n", 2);
	return (EXIT_SUCCESS);
}

char	*heredoc_init_and_get_delimiter(t_node_tree *node, t_shell *shell)
{
	if (!shell || !shell->env || shell->saved_stdin < 0)
		return (NULL);
	if (!node || !node->file)
	{
		ft_putstr_fd("minishell: Heredoc internal error\n", 2);
		return (NULL);
	}
	return (node->file);
}

// Helper 2: Child process execution logic
// Assuming handle_child_signals(), exit() are defined/available
void    execute_heredoc_child(int pipe_write_fd, int pipe_read_fd,
                            const char *delimiter, t_shell *shell)
{
    int child_stdin_backup;
    int child_stdout_backup;
    int exit_status_code;

    handle_child_signals();
    close(pipe_read_fd); 

    child_stdin_backup = dup(STDIN_FILENO);
    child_stdout_backup = dup(STDOUT_FILENO);

    if (shell->saved_stdin == -1 || shell->saved_stdout == -1)
    {
        ft_putstr_fd("minishell: critical: saved_stdin/out invalid in child.\n", STDERR_FILENO);
        if (child_stdin_backup != -1) close(child_stdin_backup);
        if (child_stdout_backup != -1) close(child_stdout_backup);
        close(pipe_write_fd);
        exit(EXIT_FAILURE);
    }
    if (dup2(shell->saved_stdin, STDIN_FILENO) == -1)
    {
        perror("minishell: heredoc_child dup2 saved_stdin");
        if (child_stdin_backup != -1) dup2(child_stdin_backup, STDIN_FILENO);
        if (child_stdout_backup != -1) dup2(child_stdout_backup, STDOUT_FILENO);
        if (child_stdin_backup != -1) close(child_stdin_backup);
        if (child_stdout_backup != -1) close(child_stdout_backup);
        close(pipe_write_fd);
        exit(EXIT_FAILURE);
    }
    if (dup2(shell->saved_stdout, STDOUT_FILENO) == -1)
    {
        perror("minishell: heredoc_child dup2 saved_stdout");
        if (child_stdin_backup != -1) dup2(child_stdin_backup, STDIN_FILENO);
        if (child_stdout_backup != -1) dup2(child_stdout_backup, STDOUT_FILENO);
        if (child_stdin_backup != -1) close(child_stdin_backup);
        if (child_stdout_backup != -1) close(child_stdout_backup);
        close(pipe_write_fd);
        exit(EXIT_FAILURE);
    }
    exit_status_code = heredoc_child_reader(pipe_write_fd, delimiter,
                                            shell->env, STDIN_FILENO);
    if (child_stdin_backup != -1) dup2(child_stdin_backup, STDIN_FILENO);
    if (child_stdout_backup != -1) dup2(child_stdout_backup, STDOUT_FILENO);
    if (child_stdin_backup != -1) close(child_stdin_backup);
    if (child_stdout_backup != -1) close(child_stdout_backup);
    get_next_line(GNL_CLEANUP); 
    minigarbege_colector();     
    close(pipe_write_fd);       
    exit(exit_status_code);
}

int  open_and_set_final_output_fd(t_node_tree *node, int *current_out_fd)
{
    int flags;
    int new_fd;

    if (*current_out_fd != -1) // An output file was previously determined
    {
        close(*current_out_fd); // Close it, this new one takes precedence
        *current_out_fd = -1;
    }
    flags = O_WRONLY | O_CREAT;
    if (node->type == AST_REDIR_OUT)
        flags |= O_TRUNC;
    else // AST_APPEND
        flags |= O_APPEND;
    new_fd = open(node->file, flags, 0644);
    if (new_fd < 0)
    {
        // perror("minishell: open output file"); // Generic
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(node->file, STDERR_FILENO); // Specific file
        ft_putstr_fd(": ", STDERR_FILENO);
        perror(NULL); // Let perror append the system error string
        return (1);
    }
    *current_out_fd = new_fd;
    return (0);
}
