/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:29:35 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/08 11:23:07 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	if (!lst || !new)
		return ;
	if (*lst)
		ft_lstlast(*lst)->next = new;
	else
		*lst = new;
}
// #include <stdio.h>
// int main(void)
// {
// 	t_list *node1 = ft_lstnew("Hello");
// 	t_list *new_node = ft_lstnew("Finish");

// 	ft_lstadd_back(&node1, new_node);

// 	printf("%s\n", (char *)node1->next->content);

// 	return (0);
// }