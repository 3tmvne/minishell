/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 18:57:54 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/09 21:59:03 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!lst || !new)
		return ;
	new->next = *lst;
	*lst = new;
}
// #include <stdio.h>
// int main(void)
// {
// 	t_list *node1 = ft_lstnew("Hello");
// 	t_list *new_node = ft_lstnew("Start");

// 	ft_lstadd_front(&node1, new_node);

// 	printf("%s\n", (char *)node1->content);

// 	return (0);
// }
