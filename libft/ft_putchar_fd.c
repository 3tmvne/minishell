/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:58 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/09 21:11:32 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
// #include <stdio.h>
// #include <fcntl.h>
// int main()
// {
// 	int fd = open("achraf.txt", O_CREAT | O_RDWR , 0666);
// 	int a = open("", O_CREAT | O_RDWR , 0000);
// 	ft_putchar_fd('a',fd);
// 	printf("%d\n", fd);
// 	printf("%d\n", a);
// 	// ft_putchar_fd('a',-1);
// 	// printf("%d\n", -1);
// }