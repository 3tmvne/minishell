/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:30:20 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/09 21:08:03 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>
#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
		{
			return ((char *)&s[i]);
		}
		i++;
	}
	if (s[i] == (char)c)
	{
		return ((char *)&s[i]);
	}
	return (NULL);
}

// int main()
// {
//     const char str[] = "Hello, world!";
//     char ch = 'o';

//     char *result = ft_strchr(str, ch);

//     if (result)
// 	{
//         printf("Le caractère '%c' a été trouvé à la position : %ld\n", ch,
// result - str);
//     }
// 	else
// 	{
//         printf("Le caractère '%c' n'a pas été trouvé dans la chaîne.\n",
// ch );
//     }
//     return (0);
// }
