#include "minishell.h"

void	env_builtin(t_env *env)
{
	t_env *current;
	
	if (!env)
		return;
		
	current = env;
	while (current)
	{
	   if (current->value)
			   printf("%s=%s\n", current->name, current->value);
		current = current->next;
	}
}
