#include "minishell.h"

void	env_builtin(t_env *env)
{
	t_env	*current;

	if (!env)
		return ;
	current = env;
	while (current)
	{
		if (current->value)
		{
			ft_putstr_fd(current->name, STDOUT_FILENO);
			if (current->value)
			{
				write(STDOUT_FILENO, "=", 1);
				ft_putstr_fd(current->value, STDOUT_FILENO);
			}
			write(STDOUT_FILENO, "\n", 1);
		}
		current = current->next;
	}
}
