#include "minishell.h"

void	env_builtin(char **env)
{
	int i = 0;
	
	if (!env)
		return;
		
	while (env[i])
	{
		printf("%s\n", env[i]);
		i++;
	}
}
