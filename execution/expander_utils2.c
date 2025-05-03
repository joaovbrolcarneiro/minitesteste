/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/04 00:03:16 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Helper for append_char: Reallocates buffer if needed */
static int	realloc_exp_buffer(char **buf, size_t len, size_t *cap)
{
	size_t	i;
	char	*new_buf;
	size_t	new_cap;

	new_cap = (*cap) * 2;
	if (new_cap == 0)
		new_cap = 128;
	new_buf = malloc(new_cap);
	if (!new_buf)
		return (perror("minishell: expander realloc"), 0);
	i = 0;
	if (*buf)
	{
		while (i < len)
		{
			new_buf[i] = (*buf)[i];
			i++;
		}
		free(*buf);
	}
	*buf = new_buf;
	*cap = new_cap;
	return (1);
}

int	append_char(char **buf, size_t *len, size_t *cap, char c)
{
	if (!buf || !len || !cap)
		return (0);
	if (!(*buf) || *len + 1 >= *cap)
	{
		if (!realloc_exp_buffer(buf, *len, cap))
			return (0);
	}
	(*buf)[(*len)] = c;
	(*len)++;
	(*buf)[(*len)] = '\0';
	return (1);
}

int	append_normal_char(t_exp_vars *v)
{
	if (!append_char(&v->result, &v->res_len, &v->res_cap, v->input[v->i]))
		return (1);
	v->i++;
	return (0);
}

long long	get_new_token_id(void)
{
	static long long	id = 0;

	return (id++);
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
	write_status = write(pipe_write_fd, line_to_write, \
							ft_strlen(line_to_write));
	if (line_to_write != line)
		free(line_to_write);
	if (write_status == -1)
	{
		perror("minishell: write heredoc pipe");
		return (1);
	}
	return (0);
}
