/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rare_candy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:39:13 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/03 17:33:35 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_part2.h"

bool	konopwd(bool cmd_exist, const char *input) /////rare_candy
{
	char			*buf;
	const size_t	size = 4096;

	buf = malloc(size);
	if (ft_strcmp(input, "pwd") == 0)
	{
		ft_printf(RDK"%s\n", getcwd(buf, size));
		free (buf);
		return (true);
	}
	free (buf);
	return (cmd_exist != false);
}

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;
	size_t	copy_size;

	if (ptr == NULL)
		return (malloc(new_size));
	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (old_size < new_size)
		copy_size = old_size;
	else
		copy_size = new_size;
	ft_memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
}
