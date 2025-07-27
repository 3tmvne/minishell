/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:31:20 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/12 22:05:43 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	d_len;
	size_t	s_len;
	size_t	i;

	if ((!dst && size == 0) || !src)
		return (ft_strlen(src));
	d_len = ft_strlen(dst);
	s_len = ft_strlen(src);
	if (size <= d_len)
	{
		return (size + s_len);
	}
	i = 0;
	while (src[i] && (d_len + i + 1) < size)
	{
		dst[d_len + i] = src[i];
		i++;
	}
	dst[d_len + i] = '\0';
	return (d_len + s_len);
}
// #include <string.h>
// #include <stdio.h>
// int main ()
// {
// 	printf("%ld\n", ft_strlcat(NULL, "elj", 0));
// 	printf("\n");
// 	printf("%d\n", strlcat(NULL, "ej", 0));

// }
