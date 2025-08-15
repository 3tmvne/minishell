#include "minishell.h"

static int is_valid_varname(const char *name)
{
	int i;

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

static char **create_sorted_env_for_export(t_env *env)
{
	// Compter toutes les variables (avec et sans valeur)
	int count = 0;
	t_env *current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	
	char **array = ft_malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	
	current = env;
	int i = 0;
	while (current)
	{
		int name_len = ft_strlen(current->name);
		if (current->value)
		{
			// Variable avec valeur : "NAME=VALUE"
			int value_len = ft_strlen(current->value);
			array[i] = ft_malloc(name_len + value_len + 2);
			if (!array[i])
			{
				return (NULL);
			}
			ft_strlcpy(array[i], current->name, name_len + 1);
			ft_strlcat(array[i], "=", name_len + 2);
			ft_strlcat(array[i], current->value, name_len + value_len + 2);
		}
		else
		{
			// Variable export-only sans valeur : "NAME"
			array[i] = ft_malloc(name_len + 1);
			if (!array[i])
			{
				// Memory allocation failed, but GC will clean up
				return (NULL);
			}
			ft_strlcpy(array[i], current->name, name_len + 1);
		}
		current = current->next;
		i++;
	}
	array[i] = NULL;
	
	// Bubble sort
	for (int x = 0; x < count - 1; x++)
	{
		for (int y = 0; y < count - x - 1; y++)
		{
			// Compare just the variable names (before '=' if present)
			char *name1 = array[y];
			char *name2 = array[y + 1];
			char *eq1 = ft_strchr(name1, '=');
			char *eq2 = ft_strchr(name2, '=');
			
			int len1 = eq1 ? (int)(eq1 - name1) : (int)ft_strlen(name1);
			int len2 = eq2 ? (int)(eq2 - name2) : (int)ft_strlen(name2);
			
			int cmp = ft_strncmp(name1, name2, (len1 > len2) ? len1 : len2);
			if (cmp > 0 || (cmp == 0 && len1 > len2))
			{
				char *temp = array[y];
				array[y] = array[y + 1];
				array[y + 1] = temp;
			}
		}
	}
	return (array);
}

static void print_exported_env(t_env *env)
{
	char **sorted_env = create_sorted_env_for_export(env);
	if (!sorted_env)
	{
		return;
	}

	for (int i = 0; sorted_env[i]; i++) {
		ft_putstr_fd("declare -x ", 1);
		char *eq = ft_strchr(sorted_env[i], '=');
		if (eq) {
			// Print name
			int name_len = eq - sorted_env[i];
			write(1, sorted_env[i], name_len);
			write(1, "=\"", 2);
			ft_putstr_fd(eq + 1, 1);
			write(1, "\"", 1);
		} else {
			// Print name only (export-only variable)
			ft_putstr_fd(sorted_env[i], 1);
		}
		write(1, "\n", 1);
	}
}

static char *extract_var_name(const char *var)
{
	int i = 0;
	while (var[i] && var[i] != '=')
		i++;
	return (ft_substr(var, 0, i));
}

/**
 * Handle a single export variable assignment or declaration
 * Returns 0 on success, 1 on error
 */
static int handle_export_var(const char *var, t_env **env)
{
	char *name;
	char *equals_pos;
	
	// Check if var name is valid
	if (var[0] == '=' || ft_isdigit(var[0]))
	{
		ft_putstr_fd("export: '", STDERR_FILENO);
		ft_putstr_fd((char*)var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (1);
	}
	
	name = extract_var_name(var);
	if (!name || name[0] == '\0')
	{
		ft_putstr_fd("export: '", STDERR_FILENO);
		ft_putstr_fd((char*)var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		// name is managed by GC
		return (1);
	}

	if (!is_valid_varname(name))
	{
		ft_putstr_fd("export: '", STDERR_FILENO);
		ft_putstr_fd((char*)var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		// name is managed by GC
		return (1);
	}
	
	equals_pos = ft_strchr(var, '=');
	if (equals_pos)
	{
		// Variable has a value (could be empty: VAR=)
		char *value = equals_pos + 1;
        
        // NOTE: We no longer need to handle empty quotes here because:
        // 1. expand_tokens_selective already concatenated tokens and skipped empty quoted tokens
        // 2. The tokenizer properly handles empty quotes
        // So at this point, 'value' already has the correct concatenated value
        
        // Just trim trailing newlines if any
        char *parsed_value = ft_strdup(value);
        // ft_strdup already uses ft_malloc internally
        if (!parsed_value)
        {
            // name is managed by GC
            return (1); // Return error code if we can't allocate memory
        }
        
        int len = ft_strlen(parsed_value);
        while (len > 0 && (parsed_value[len-1] == '\n' || parsed_value[len-1] == '\r'))
        {
            parsed_value[len-1] = '\0';
            len--;
        }
        
        // Set the variable in the environment
        set_env_var(env, name, parsed_value);
        // parsed_value and name are managed by GC
        return (0);
	}
	else
	{
		// Variable without value, just mark for export
		t_env *existing = find_env_var(*env, name);
		if (!existing)
		{
			// Create new export-only variable (value = NULL)
			t_env *new_var = create_env_node(name, NULL);
			if (new_var)
			{
				// Add to end of list for consistent order
				t_env *cur = *env;
				if (!cur) {
					*env = new_var;
				} else {
					while (cur->next)
						cur = cur->next;
					cur->next = new_var;
				}
			}
		}
		// If variable exists, do nothing
        // name is managed by GC
        return (0);
	}
}

/**
 * Implements the export builtin command
 * This function sets/updates environment variables or marks them for export
 * Returns the exit status (0 for success, 1 for error)
 */
int export_builtin(t_cmd *cmd, t_env **env)
{
    int i;
    int exit_status = 0;
    
    if (!cmd->args[1])
    {
        // No arguments: print all environment variables
        print_exported_env(*env);
        return (0);
    }

    // Process each argument
    i = 1;
    while (cmd->args[i])
    {
        // Skip completely empty arguments
        if (!cmd->args[i] || cmd->args[i][0] == '\0')
        {
            i++;
            continue;
        }
        
        // Process this argument
        // Note: expand_tokens_selective has already concatenated tokens with proper quote handling
        // so we don't need special handling for arguments split across multiple tokens
        int result = handle_export_var(cmd->args[i], env);
        if (result != 0)
            exit_status = result;
        i++;
    }
    
    return (exit_status);
}

char **get_filtered_env_list(t_env *env)
{
	int count = 0;
	t_env *current = env;
	
	// Count variables that have actual values (pas les export-only)
	while (current)
	{
		if (current->value)  // Seulement si la variable a une valeur
			count++;
		current = current->next;
	}
	
	char **filtered_env = ft_malloc(sizeof(char *) * (count + 1));
	if (!filtered_env)
		return (NULL);
	
	current = env;
	int i = 0;
	while (current)
	{
		if (current->value)  // Seulement si la variable a une valeur
		{
			int name_len = ft_strlen(current->name);
			int value_len = ft_strlen(current->value);
			filtered_env[i] = ft_malloc(name_len + value_len + 2);
			if (filtered_env[i])
			{
				ft_strlcpy(filtered_env[i], current->name, name_len + 1);
				ft_strlcat(filtered_env[i], "=", name_len + 2);
				ft_strlcat(filtered_env[i], current->value, name_len + value_len + 2);
				i++;
			}
		}
		current = current->next;
	}
	filtered_env[i] = NULL;
	
	
	return (filtered_env);
}
