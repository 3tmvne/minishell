#include "minishell.h"

typedef struct s_gc
{
	void		*ptr;
	int         flag;
	struct s_gc	*next;
}				t_gc;

static t_gc	**get_gc_head(void)
{
	static t_gc	*gc_head = NULL;

	return (&gc_head);
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

void	free_gc(void)
{
	t_gc	**gc_head;
	t_gc	*current;
	t_gc	*next;

	gc_head = get_gc_head();
	current = *gc_head;
	while (current)
	{
		next = current->next;
		if (current->flag == 0) {
			free(current->ptr);
			free(current);
		} else {
			// Do not free ptr, only free the node struct
			free(current);
		}
		current = next;
	}
	*gc_head = NULL; // Reset the GC list
}

// Free only nodes with flag == 0 (after each command)
void	free_gc_flag0(void)
{
	t_gc   **gc_head;
	t_gc   *current;
	t_gc   *prev;

	gc_head = get_gc_head();
	current = *gc_head;
	prev = NULL;
	while (current)
	{
		t_gc *next = current->next;
		if (current->flag == 0) {
			free(current->ptr);
			if (prev)
				prev->next = next;
			else
				*gc_head = next;
			free(current);
			current = next;
			continue;
		}
		prev = current;
		current = next;
	}
}

// Free all nodes and all memory (on exit)
void	free_gc_all(void)
{
	t_gc   **gc_head;
	t_gc   *current;
	t_gc   *next;

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
