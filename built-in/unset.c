#include "minishell.h"

/**
 * Checks if a variable name is valid
 * Valid names must start with a letter or underscore
 * and can only contain alphanumeric characters and underscores
 */
static int	is_valid_varname(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (name[0] != '_' && !ft_isalpha(name[0]))
		return (0);
	i = 0;
	while (name[i])
	{
		if (name[i] != '_' && !ft_isalnum(name[i]))
			return (0);
		i++;
	}
	return (1);
}
/**
 * Implements the unset builtin command
 * Removes variables from the environment linked list structure
 * Returns the exit status (0 for success, 1 for error)
 */
int	unset_builtin(t_cmd *cmd, t_env **env)
{
	int i;
	int exit_status;

	if (!cmd || !cmd->args || !env)
		return (1); // Error

	if (!cmd->args[1])
		return (0); // No arguments, nothing to do

	// Process each argument
	i = 1;
	exit_status = 0;
	while (cmd->args[i])
	{
		if (is_valid_varname(cmd->args[i]))
		{
			unset_env_var(env, cmd->args[i]);
		}
		else
		{
			ft_putstr_fd("unset: '", STDERR_FILENO);
			ft_putstr_fd(cmd->args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			exit_status = 1; // Set error status if any argument is invalid
		}
		i++;
	}

	return (exit_status);
}