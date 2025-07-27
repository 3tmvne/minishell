/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 20:33:15 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/12 02:37:18 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*node;

	if (!lst || !del)
		return ;
	while (*lst)
	{
		node = (*lst)->next;
		ft_lstdelone(*lst, del);
		*lst = node;
	}
}
// #include <stdio.h>

// void	del(void *content)
// {
// 	free(content);
// }

// int main(void)
// {
// 	t_list *node1 = ft_lstnew(ft_strdup("Hello"));
// 	t_list *node2 = ft_lstnew(ft_strdup("World"));
// 	t_list *node3 = ft_lstnew(ft_strdup("!"));

// 	node1->next = node2;
// 	node2->next = node3;

// 	ft_lstclear(&node1, del);

// 	if (!node1)
// 		printf("mkyn walo\n");

// 	return (0);
// }
