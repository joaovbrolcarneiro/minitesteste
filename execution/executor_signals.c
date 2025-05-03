/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 19:58:37 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Sets signal handlers for the main shell (parent process) */
void	handle_parent_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/* --- Heredoc functions (Should ideally be in executor_redir_utils.c) --- */

/* Helper: Checks if line matches heredoc delimiter */
static bool	is_heredoc_delim(const char *line, const char *delimiter)
{
	size_t	len;
	bool	match_name;
	bool	match_end;

	len = ft_strlen(delimiter);
	match_name = (ft_strncmp(line, delimiter, len) == 0);
	match_end = (line[len] == '\n' || line[len] == '\0');
	return (match_name && match_end);
}

/* Helper: Reads heredoc lines until delimiter or EOF */
/* Returns 0 on success, -1 on write error */
static int	read_heredoc_lines(int pipe_write_fd, const char *delimiter, \
								char **env)
{
	char	*line;
	bool	expand;
	int		status;

	expand = true;
	status = 0;
	while (1)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = get_next_line(STDIN_FILENO);
		if (line == NULL)
		{
			ft_putstr_fd("minishell: warning: heredoc EOF (wanted `", 2);
			ft_putstr_fd((char *)delimiter, 2);
			ft_putstr_fd("')\n", 2);
			return (0);
		}
		if (is_heredoc_delim(line, delimiter))
		{
			free(line); // free?
			return (0);
		}
		status = process_heredoc_line(line, pipe_write_fd, env, expand);
		free(line); // free?
		if (status != 0)
			return (1);
	}
}

/* Reads heredoc input (Split to satisfy Norminette line count) */
int	read_heredoc_input(int pipe_write_fd, const char *delimiter, char **env)
{
	return (read_heredoc_lines(pipe_write_fd, delimiter, env));
}

int	handle_redirections(t_node_tree *node, t_shell *shell)
{
	if (!node)
		return (0);
	if (node->type == AST_REDIR_IN)
		return (handle_redir_in(node));
	else if (node->type == AST_REDIR_OUT)
		return (handle_redir_out(node));
	else if (node->type == AST_APPEND)
		return (handle_append(node));
	else if (node->type == AST_HEREDOC)
		return (handle_heredoc(node, shell));
	return (1);
}

int	process_heredoc_line(char *line, int pipe_write_fd, \
								char **env, bool expand)
{
	char	*line_to_write;
	int		write_status;

	line_to_write = line;
	if (expand)
	{
		line_to_write = expand_variables(line, env);
		if (!line_to_write)
			return (1);
	}
	write_status = write(pipe_write_fd, line_to_write, ft_strlen(line_to_write));
	if (line_to_write != line)
		free(line_to_write); // free?
	if (write_status == -1)
	{
		perror("minishell: write heredoc pipe");
		return (1);
	}
	return (0);
}