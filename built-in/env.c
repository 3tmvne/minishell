#include "minishell.h"

void	env_builtin(t_shell_state *shell)
{
	char **filtered_env;
	int i = 0;
	
	if (!shell || !shell->env)
	{
		shell->last_exit_status = 1;
		return;
	}
	
	// Get environment without export-only variables
	filtered_env = get_filtered_env(shell->env);
	if (!filtered_env)
	{
		shell->last_exit_status = 1;
		return;
	}
		
	while (filtered_env[i])
	{
		printf("%s\n", filtered_env[i]);
		i++;
	}
	
	// Free filtered environment
	i = 0;
	while (filtered_env[i])
		free(filtered_env[i++]);
	free(filtered_env);
	
	shell->last_exit_status = 0;
}
