/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:31:23 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/09 21:08:23 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>
#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size != 0)
	{
		while (src[i] != '\0' && i < (size - 1))
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}

// #include <string.h>
// int main ()
// {
// 	char dst[10] = " hhjgcvcu";
// 	printf("%ld\n%s\n", strlcpy(dst, "eljkf", 6), dst);
// }
/*int main()
{
	char dst[10];

	printf("%zu\n", ft_strlcpy(dst, "eljkf", 5));
	printf("dst: %s\n", dst);

	return (0);
}*/
