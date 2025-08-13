/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:30:15 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/13 01:31:38 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, char c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static int	ft_strlenc(const char *s, char c)
{
	int	i;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	int		i;
	int		j;

	if (!s)
		return (NULL);
	(1) && (i = 0, split = (char **)ft_malloc((count_words(s, c) + 1)
			* sizeof(char *)));
	if (!split)
		return (NULL);
	while (*s != '\0')
	{
		if (*s != c)
		{
			(1) && (j = 0, split[i] = ft_malloc((ft_strlenc(s, c) + 1)));
			if (!split[i])
				return (NULL);
			while (*s && *s != c)
				split[i][j++] = *s++;
			split[i++][j] = '\0';
		}
		else
			s++;
	}
	return (split[i] = NULL, split);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	str[] = "   aaaaaafutur ea is aloaadingaaaaaaa ";
// 	char	**split;
// 	int		i;

// 	split = ft_split(str, 'a');
// 	i = 0;
// 	while (split[i] != 0)
// 	{
// 		printf("%s\n", split[i]);
// 		i++;
// 	}
// 	// printf("%s \n", split[i]);
// 	free(split);
// }
