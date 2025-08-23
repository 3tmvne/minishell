/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   g_b.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:42:09 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/23 17:31:41 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_gc	**get_gc_head(void)
{
	static t_gc	*gc_head = NULL;

	return (&gc_head);
}

void	add_flag_to_gc(t_env *env_node)
{
	t_gc	**gc_head;
	t_gc	*cur;

	gc_head = get_gc_head();
	cur = *gc_head;
	while (cur)
	{
		if (cur->ptr == (void *)env_node)
		{
			cur->flag = 1;
			break ;
		}
		cur = cur->next;
	}
}

void	add_to_gc(void *ptr)
{
	t_gc	**gc_head;
	t_gc	*node;

	gc_head = get_gc_head();
	node = malloc(sizeof(t_gc));
	if (!node)
		return ;
	node->ptr = ptr;
	node->flag = 0;
	node->next = *gc_head;
	*gc_head = node;
}

void	free_gc_flag0(void)
{
	t_gc	**gc_head;
	t_gc	*current;
	t_gc	*prev;
	t_gc	*next;

	gc_head = get_gc_head();
	current = *gc_head;
	prev = NULL;
	while (current)
	{
		next = current->next;
		if (current->flag == 0)
		{
			free(current->ptr);
			if (prev)
				prev->next = next;
			else
				*gc_head = next;
			free(current);
			current = next;
			continue ;
		}
		prev = current;
		current = next;
	}
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
		free(current->ptr);
		free(current);
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
