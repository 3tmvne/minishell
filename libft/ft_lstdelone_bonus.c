/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:15:47 by aregragu          #+#    #+#             */
/*   Updated: 2024/11/12 02:37:37 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!lst || !del)
		return ;
	(*del)(lst->content);
	free(lst);
}
// #include <stdio.h>

// void	del(void *content)
// {
// 	free(content);
// }

// int main(void)
// {
// 	t_list *node = ft_lstnew(ft_strdup("Hello"));

// 	ft_lstdelone(node, del);

// 	if (!node)
// 		printf("mcha klchi\n");

// 	else
// 		printf("m freeye.\n");

// 	return (0);
// }
