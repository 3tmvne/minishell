/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:30:15 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/17 01:23:03 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	cd(const char *s, char c)
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

static void	ft_free(char **s, int i)
{
	if (i == 0)
	{
		free(s);
		return ;
	}
	while (--i >= 0)
		free(s[i]);
	free(s);
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
	(1) && (i = 0, split = (char **)malloc((cd(s, c) + 1) * sizeof(char *)));
	if (!split)
		return (NULL);
	while (*s != '\0')
	{
		if (*s != c)
		{
			(1) && (j = 0, split[i] = malloc((ft_strlenc(s, c) + 1)));
			if (!split[i])
				return (ft_free(split, i), NULL);
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
// 	char	str[] = "   aaaaaafutur ea is aloaadingaaaaaaa    ";
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
