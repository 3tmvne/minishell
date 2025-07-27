/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:37 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/08 11:21:58 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next != NULL)
		lst = lst->next;
	return (lst);
}
// #include <stdio.h>
// int main ()
// {
// 	t_list *lst;
// 	t_list *node1 = ft_lstnew("123");
// 	t_list *node2 = ft_lstnew("gth");
// 	t_list *node3 = ft_lstnew("455");
// 	node1 ->next = node2;
// 	node2 ->next = node3;
// 	lst = node1;
// 	t_list *laast = ft_lstlast(lst);
// 	printf("%s \n",(char*)laast->content);
// }