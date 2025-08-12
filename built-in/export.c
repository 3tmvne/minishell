#include "minishell.h"

/**
 * Check if a variable has only a name (export VAR without value)
 * We distinguish between:
 * - "VAR" = export-only (no value, won't appear in env)
 * - "VAR=" = empty value (will appear in env)
 */
static int is_export_only_var(const char *var)
{
	char *equals_pos;
	
	equals_pos = ft_strchr(var, '=');
	if (!equals_pos)
		return (1); // No '=', so it's export-only
	
	return (0); // Has '=', so it has a value (even if empty)
}

/**
 * Checks if a variable name is valid
 * Valid names must start with a letter or underscore
 * and can only contain alphanumeric characters and underscores
 */
/*
static int	is_valid_varname(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (name[0] != '_' && !ft_isalpha(name[0]))
		return (0);
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (name[i] != '_' && !ft_isalnum(name[i]))
			return (0);
		i++;
	}
	return (1);
}
*/

/**
 * Count number of environment variables
 */
static int	count_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
		i++;
	return (i);
}

/**
 * Creates a copy of the environment variables sorted alphabetically
 */
static char	**create_sorted_env(char **env)
{
	char	**sorted_env;
	int		env_size;
	int		i;
	int		j;
	char	*temp;

	env_size = count_env(env);
	sorted_env = (char **)malloc(sizeof(char *) * (env_size + 1));
	if (!sorted_env)
		return (NULL);
	i = -1;
	while (++i < env_size)
		sorted_env[i] = ft_strdup(env[i]);
	sorted_env[env_size] = NULL;
	i = -1;
	while (++i < env_size - 1)
	{
		j = -1;
		while (++j < env_size - i - 1)
		{
			if (strcmp(sorted_env[j], sorted_env[j + 1]) > 0)
			{
				temp = sorted_env[j];
				sorted_env[j] = sorted_env[j + 1];
				sorted_env[j + 1] = temp;
			}
		}
	}
	return (sorted_env);
}

/*
 * Adds a new variable to the environment array
 */
/*
static char	**add_to_env_array(char **env, char *var)
{
	char	**new_env;
	int		size;
	int		i;

	size = count_env(env);
	new_env = (char **)malloc(sizeof(char *) * (size + 2));
	if (!new_env)
		return (NULL);
	i = -1;
	while (++i < size)
		new_env[i] = ft_strdup(env[i]);
	new_env[size] = ft_strdup(var);
	new_env[size + 1] = NULL;
	i = -1;
	while (env[++i])
		free(env[i]);
	free(env);
	return (new_env);
}
*/

/**
 * Prints environment variables in the export format: declare -x NAME="VALUE"
 */
static void	print_exported_env(char **env)
{
	char	**sorted_env;
	int		i, j;
	
	sorted_env = create_sorted_env(env);
	if (!sorted_env)
		return;
	
	// Print all variables
	i = 0;
	while (sorted_env[i])
	{
		ft_putstr_fd("declare -x ", 1);
		j = 0;
		
		// Print variable name
		while (sorted_env[i][j] && sorted_env[i][j] != '=')
			write(1, &sorted_env[i][j++], 1);
		
		// Print value if it exists
		if (sorted_env[i][j] == '=')
		{
			// Variable has a value (même si vide)
			write(1, "=\"", 2);
			ft_putstr_fd(&sorted_env[i][j + 1], 1);
			write(1, "\"", 1);
		}
		// Si pas de '=', c'est une variable export-only (pas de valeur)
		write(1, "\n", 1);
		i++;
	}
	
	// Free sorted environment
	i = 0;
	while (sorted_env[i])
		free(sorted_env[i++]);
	free(sorted_env);
}

/*
 * Extract variable name from a string (before '=' character)
 */
/*
static char	*extract_var_name(const char *var)
{
	int		i;
	char	*name;

	i = 0;
	while (var[i] && var[i] != '=')
		i++;
	name = ft_substr(var, 0, i);
	return (name);
}
*/

/*
 * Process a single export variable
 * Returns updated environment array
 */
/*
static char **process_export_var(char **env, char *var)
{
	char	*name;
	char	*existing_value;
	char	*new_var;
	int		has_equals;
	int		i;

	// Check if variable has '=' sign
	has_equals = 0;
	i = 0;
	while (var[i] && var[i] != '=')
		i++;
	if (var[i] == '=')
		has_equals = 1;
	
	name = extract_var_name(var);
	if (!name)
		return (env);
	
	// Check if variable exists
	existing_value = get_env_value(name, env);
	
	if (has_equals)
	{
		// Variable has a value (could be empty: VAR= )
		update_env_value(name, var + i + 1, env);
	}
	else if (!existing_value)
	{
		// Variable doesn't exist and no value provided
		// Store as "VAR" (exported but no value - different from VAR=)
		// We use a special marker to distinguish from VAR=""
		new_var = ft_strjoin(name, "");  // Just the name, no "="
		if (new_var)
		{
			env = add_to_env_array(env, new_var);
			free(new_var);
		}
	}
	// If variable exists and no new value provided, do nothing
	
	free(name);
	return (env);
}
*/

/**
 * Implements the export builtin command
 * Adds variables to the environment or displays all variables
 */
void export_builtin(t_cmd *cmd, char **env)
{
	int i;
	
	if (!cmd->args[1])
	{
		// No arguments: print all environment variables
		print_exported_env(env);
		return;
	}
	
	// Process each argument
	i = 1;
	while (cmd->args[i])
	{
		// For now, a simple implementation
		// This would need more complex logic for proper export behavior
		if (ft_strchr(cmd->args[i], '='))
		{
			// Has '=', so it's a variable assignment
			// This should update the environment
			// For now, just print it
			printf("export: would set %s\n", cmd->args[i]);
		}
		else
		{
			// No '=', so it's export-only
			printf("export: would mark %s for export\n", cmd->args[i]);
		}
		i++;
	}
}

/**
 * Creates a filtered environment without export-only variables (no value)
 * This is used for the 'env' command and child processes
 */
char **get_filtered_env(char **env)
{
	char **filtered_env;
	int count = 0;
	int i, j;
	
	// Count variables that should be included (have actual values)
	i = 0;
	while (env[i])
	{
		if (!is_export_only_var(env[i]))
			count++;
		i++;
	}
	
	// Allocate filtered environment
	filtered_env = malloc(sizeof(char *) * (count + 1));
	if (!filtered_env)
		return (NULL);
	
	// Copy variables with actual values
	i = 0;
	j = 0; 
	while (env[i])
	{
		if (!is_export_only_var(env[i]))
		{
			filtered_env[j] = ft_strdup(env[i]);
			j++;
		}
		i++;
	}
	filtered_env[j] = NULL;
	
	return (filtered_env);
}
