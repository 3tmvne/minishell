/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 11:24:12 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/08 20:00:56 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	t_list	*ptr_lst;

	if (!lst)
		return ;
	ptr_lst = lst;
	while (ptr_lst != NULL)
	{
		(*f)(ptr_lst->content);
		ptr_lst = ptr_lst->next;
	}
}
// #include <stdio.h>

// void	print_content(void *content)
// {
// 	printf("%s\n", (char *)content);
// }

// int	main(void)
// {
// 	t_list	*node1;
// 	t_list	*node2;
// 	t_list	*node3;

// 	node1 = ft_lstnew(ft_strdup("Future"));
// 	node2 = ft_lstnew(ft_strdup("is"));
// 	node3 = ft_lstnew(ft_strdup("loading"));
// 	node1->next = node2;
// 	node2->next = node3;
// 	ft_lstiter(node1, print_content);
// 	return (0);
// }
