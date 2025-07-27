/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:31 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/12 21:39:31 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>
#include "libft.h"

int	nb_len(int n)
{
	int	l;

	l = 0;
	if (n == 0)
		return (1);
	if (n < 0)
	{
		l++;
		n = -n;
	}
	while (n)
	{
		l++;
		n = n / 10;
	}
	return (l);
}

char	*ft_itoa(int n)
{
	long	c;
	int		size;
	char	*itoa;

	c = n;
	size = nb_len(c);
	itoa = malloc(size + 1);
	if (!itoa)
		return (NULL);
	itoa[size] = '\0';
	if (c == 0)
	{
		return (itoa[0] = '0', itoa);
	}
	if (c < 0)
	{
		itoa[0] = '-';
		c = -c;
	}
	while (c)
	{
		itoa[--size] = (c % 10) + '0';
		c = c / 10;
	}
	return (itoa);
}

// #include <stdlib.h>

// int	main(void)
// {
// 	printf("%s \n", ft_itoa(0));
// 	return (0);
// }
