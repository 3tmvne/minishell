/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:46 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/12 20:08:17 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <string.h>
// #include <stdio.h>
// #include <stddef.h>
#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (ptr[i] == (unsigned char)c)
			return ((unsigned char *)s + i);
		i++;
	}
	return (NULL);
}
// int main()
// {
//     const char *str = "achraf";
//     char c = 'r';
//     size_t n = 5;
//    char *result = ft_memchr(str, c, n);
// 		//char *r = memchr(str, c, n);

//     //printf("%s\n",r);
//     printf("%s\n", result );

//     return (0);
// }
