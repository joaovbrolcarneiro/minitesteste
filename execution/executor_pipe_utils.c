/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 16:51:21 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Executes left side of pipe in child process */
void	left_child(t_shell *shell, t_node_tree *left, int pipefd[2])
{
	int	exit_status;

	close(pipefd[0]);
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 stdout left_child");
		exit(EXIT_FAILURE);
	}
	close(pipefd[1]);
	exit_status = execute_ast(shell, left);
	exit(exit_status);
}

/* Executes right side of pipe in child process */
void	right_child(t_shell *shell, t_node_tree *right, int pipefd[2])
{
	int	exit_status;

	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 stdin right_child");
		exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	exit_status = execute_ast(shell, right);
	exit(exit_status);
}

/* Forks process for the left side of a pipe */
int	fork_left(t_shell *shell, t_node_tree *node, int pipefd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		left_child(shell, node->left, pipefd);
	}
	else if (pid < 0)
	{
		perror("minishell: fork");
		return (-1);
	}
	return (pid);
}

/* Forks process for the right side of a pipe */
int	fork_right(t_shell *shell, t_node_tree *node, int pipefd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		right_child(shell, node->right, pipefd);
	}
	else if (pid < 0)
	{
		perror("minishell: fork");
		return (-1);
	}
	return (pid);
}
