/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:30:25 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/09 21:08:10 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	if (!s || !f)
		return ;
	i = 0;
	while (s[i] != '\0')
	{
		f(i, &s[i]);
		i++;
	}
}

// void f(unsigned int i, char *s)
// {
//     if (i % 2 == 0)
//     {
//         *s = 'A';
//     }
// }

// void d(unsigned int i, char *s)
// {
//     if (i % 2 == 0)
//     {
//         *s = 'B';
//     }
// }

// void g(unsigned int i, char *s)
// {
//     if (i % 2 == 0)
//     {
//         *s = 'C';
//     }
// }
// #include <stdio.h>
// int main ()
// {
//     char str[] = "123456";

//     ft_striteri(str, f);
//     printf("%s \n", str);
//     char str1[] = "123456";
//     ft_striteri(str1, d);
//     printf("%s \n", str1);
//     char str2[] = "123456";
//     ft_striteri(str2, g);
//     printf("%s \n", str2);
// }