/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:43 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/06 21:17:51 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	while (lst != NULL)
	{
		count++;
		lst = lst->next;
	}
	return (count);
}
//#include <stdio.h>
// int main()
// {
// 	struct s_list hhhh;
// 	printf("%ld \n", sizeof(hhhh));
// }
// #include <stdlib.h>
// typedef struct lst {
// 	int age;
// 	char *name;
// 	char *d_birth;
//     struct lst *next;
// } t_list;
// int main()
// {
// 	t_list list1;
// 	t_list list2;
// 	list1.age = 21;
// 	list1.name = "mohamed";
// 	list1.d_birth = "20/10/2002";
// 	list2.age = 19;
// 	list2.name = "hippo";
// 	list2.d_birth = "29/10/2002";
// 	list1.next = &list2;
// 	printf("%d \n", list1.age);
// 	printf("%d \n", list1.next->age);
// }
