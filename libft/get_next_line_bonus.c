/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 14:03:18 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 15:51:42 by jbrol-ca         ###   ########.fr       */
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
	static char	*buffer; // Static buffer for leftovers

	// Handle special cleanup call
	if (fd == GNL_CLEANUP)
	{
		if (buffer)
			free(buffer); // Free the static buffer if it exists
		buffer = NULL;    // Reset the static pointer
		return (NULL);    // Return NULL for cleanup call
	}

	// Standard GNL validation
	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL); // Invalid input, don't touch static buffer yet

	// Read data into buffer, potentially reallocating and freeing old buffer internally
	buffer = ft_take_date(fd, buffer);
	if (!buffer) // Error during read or EOF before any data was buffered
		return (NULL);

	// Extract the line to return (allocates 'line' with std malloc)
	line = ft_take_line(buffer);

	// Update static buffer with leftovers.
	// ft_rebuffer frees the buffer passed to it and returns a new buffer
	// (allocated with std malloc) or NULL if no leftovers/error.
	buffer = ft_rebuffer(buffer);

	// If ft_take_line failed (e.g., buffer was empty or only contained '\n'),
	// line is NULL. In this case, ft_rebuffer should have also received an
	// effectively empty buffer, freed it, and returned NULL. The static buffer
	// pointer is now correctly NULL.
	if (!line)
		return (NULL);

	// Return the malloc'd line. Caller must free it.
	// The static buffer now holds only leftovers or is NULL.
	return (line);
}
