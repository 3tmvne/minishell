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
 * Check if a variable exists in the environment
 * Returns the index of the variable if found, -1 otherwise
 */
static int	env_var_exist(const char *name, char **env)
{
	int		i;
	int		name_len;
	
	if (!name || !env)
		return (-1);
	
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		// Check if this env entry starts with name followed by '='
		if (ft_strncmp(env[i], name, name_len) == 0 && 
		   (env[i][name_len] == '=' || env[i][name_len] == '\0'))
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

/**
 * Remove a variable from the environment
 * Creates a new environment array without the specified variable
 */
static char **remove_var_from_env(const char *name, char **env)
{
	char	**new_env;
	int		var_pos;
	int		env_size;
	int		i;
	int		j;
	
	var_pos = env_var_exist(name, env);
	if (var_pos == -1)  // Variable doesn't exist
		return (env);
	
	// Count environment size
	env_size = 0;
	while (env[env_size])
		env_size++;
	
	// Create new environment without the variable
	new_env = (char **)malloc(sizeof(char *) * env_size); // One less than before
	if (!new_env)
		return (env);  // If malloc fails, return original env
	
	// Copy everything except the variable to remove
	i = 0;
	j = 0;
	while (env[i])
	{
		if (i != var_pos)
		{
			new_env[j] = ft_strdup(env[i]);
			j++;
		}
		i++;
	}
	new_env[j] = NULL;  // Null terminate the array
	
	// Free old environment
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
	
	return (new_env);
}

/**
 * Process a single unset argument
 * Returns 0 on success, -1 if the variable name is invalid
 */
static int	unset_arg(char *arg, char ***env)
{
	if (!is_valid_varname(arg))
		return (-1);
	
	// Remove from environment if it exists
	*env = remove_var_from_env(arg, *env);
	
	return (0);
}

/**
 * Implements the unset builtin command
 * Removes variables from the environment
 */
void	unset_builtin(t_token *tokens, t_shell_state *shell)
{
	t_token	*current;
	int		status;
	
	current = tokens->next; // Skip "unset" command
	
	if (!current || current->type != WORD)
	{
		shell->last_exit_status = 0;
		return;
	}
	
	// Traiter chaque argument token
	while (current && current->type == WORD)
	{
		status = unset_arg(current->value, &shell->env);
		if (status == -1)
		{
			ft_putstr_fd("minishell: unset: '", 2);
			ft_putstr_fd(current->value, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			shell->last_exit_status = 1;
		}
		current = current->next;
	}
}