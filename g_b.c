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


void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	add_to_gc(ptr);
	return (ptr);
}
