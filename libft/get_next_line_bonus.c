/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 14:03:18 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 23:11:12 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*ft_take_line(char *buffer)
{
	int		i;
	char	*line;

	i = 0;
	if (!buffer[i])
		return (NULL);
	while (buffer[i] != '\n' && buffer[i])
		i++;
	line = ft_calloc_gnl((i + 2), sizeof(char));
	if (!line)
		return (NULL);
	i = 0;
	while (buffer[i] != '\n' && buffer[i])
	{
		line[i] = buffer[i];
		i++;
	}
	if (buffer[i] == '\n')
	{
		line[i] = buffer[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

char	*ft_take_date(int fd, char *buffer)
{
	char	*cache;
	int		cache_len;

	cache = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!cache && buffer)
		return (free(buffer), NULL);
	else if (!cache)
		return (NULL);
	cache_len = 1;
	while (!ft_strchr_gnl(buffer, '\n') && cache_len > 0)
	{
		cache_len = read(fd, cache, BUFFER_SIZE);
		if (cache_len < 0)
			return (free(cache), free(buffer), NULL);
		cache[cache_len] = '\0';
		buffer = ft_strjoin_gnl(buffer, cache);
		if (!buffer)
			return (free(cache), NULL);
	}
	return (free(cache), buffer);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*buffer;

	if (fd == GNL_CLEANUP)
	{
		if (buffer)
			free(buffer);
		buffer = NULL;
		return (NULL);
	}
	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	buffer = ft_take_date(fd, buffer);
	if (!buffer)
		return (NULL);
	line = ft_take_line(buffer);
	buffer = ft_rebuffer(buffer);
	if (!line)
		return (NULL);
	return (line);
}
