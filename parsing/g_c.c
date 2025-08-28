/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   g_c.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:42:09 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/26 01:40:41 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_gc	**get_gc_head(void)
{
	static t_gc	*gc_head = NULL;

	return (&gc_head);
}

void	add_to_gc(void *ptr)
{
	t_gc	**gc_head;
	t_gc	*node;
	t_gc	*current;

	if (!ptr)
		return ;
	gc_head = get_gc_head();
	current = *gc_head;
	while (current)
	{
		if (current->ptr == ptr)
			return ;
		current = current->next;
	}
	node = malloc(sizeof(t_gc));
	if (!node)
	{
		free(ptr);
		return ;
	}
	node->ptr = ptr;
	node->next = *gc_head;
	*gc_head = node;
}

void	free_gc_all(void)
{
	t_gc	**gc_head;
	t_gc	*current;
	t_gc	*next;

	gc_head = get_gc_head();
	current = *gc_head;
	while (current)
	{
		next = current->next;
		if (current->ptr)
		{
			free(current->ptr);
			current->ptr = NULL;
			free(current);
		}
		current = next;
	}
	*gc_head = NULL;
}

void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	add_to_gc(ptr);
	return (ptr);
}
