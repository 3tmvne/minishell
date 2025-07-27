/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 21:15:21 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/16 21:27:07 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*s;
	size_t	start;
	size_t	end;
	size_t	i;

	i = 0;
	start = 0;
	if (!s1 || !set)
		return (NULL);
	end = ft_strlen(s1);
	while (s1[start] && (ft_strchr((char *)set, s1[start])))
		start++;
	while (end > start && (ft_strchr((char *)set, s1[end - 1])))
		end--;
	s = (char *)malloc(end - start + 1);
	if (!s)
		return (NULL);
	while (s1[start] && start < end)
	{
		s[i] = s1[start];
		start++;
		i++;
	}
	return (s[i] = 0, s);
}

// int main()
// {
//     char const *str = "isssssssssssiiiiifutureisloadingisachrafisis";
//     const char *r = ft_strtrim(str,"isf");

//     printf("%s\n", r);
// }
