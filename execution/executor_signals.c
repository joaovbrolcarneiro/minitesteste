/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 16:24:50 by jbrol-ca         ###   ########.fr       */
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
bool	is_heredoc_delim(const char *line, const char *delimiter)
{
	size_t	delim_len;
	size_t	line_len;

	if (!line || !delimiter)
		return (false);
	delim_len = ft_strlen(delimiter);
	line_len = ft_strlen(line);
	if (line_len > 0 && line[line_len - 1] == '\n')
	{
		line_len--;
	}
	if (line_len != delim_len)
		return (false);
	if (ft_strncmp(line, delimiter, delim_len) == 0)
		return (true);
	return (false);
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
			return (free(line), 0);
		status = process_heredoc_line(line, pipe_write_fd, env, expand);
		free(line);
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
