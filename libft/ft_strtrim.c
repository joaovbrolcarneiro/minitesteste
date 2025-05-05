/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 07:24:21 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 20:09:21 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_ischarset(const char *set, char c)
{
	while (*set)
	{
		if (*set == c)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;

	if (!s1 || !set)
		return (NULL);

	start = 0;
	while (s1[start] && ft_ischarset(set, s1[start]))
		start++;

	end = ft_strlen(s1); // Get length (index after last char)
	while (end > start && ft_ischarset(set, s1[end - 1])) // Check char before end
		end--;

	return (ft_substr(s1, start, end - start));
}

