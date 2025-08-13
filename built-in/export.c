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
	
	char **array = malloc(sizeof(char *) * (count + 1));
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
			array[i] = malloc(name_len + value_len + 2);
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
			array[i] = malloc(name_len + 1);
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

static void handle_export_var(const char *var, t_env **env)
{
	char *name;
	char *equals_pos;
	
	// Check if var name is valid
	if (var[0] == '=' || ft_isdigit(var[0]))
	{
		printf("export: '%s': not a valid identifier\n", var);
		return;
	}
	
	name = extract_var_name(var);
	if (!name || name[0] == '\0')
	{
		printf("export: '%s': not a valid identifier\n", var);
		return;
	}

	if (!is_valid_varname(name))
	{
		printf("export: '%s': not a valid identifier\n", var);
		return;
	}
	
	equals_pos = ft_strchr(var, '=');
	if (equals_pos)
	{
		// Variable has a value (could be empty: VAR=)
		char *value = equals_pos + 1;
		// Remove trailing newlines from value
		char *clean_value = ft_strdup(value);
		if (clean_value)
		{
			int len = ft_strlen(clean_value);
			while (len > 0 && (clean_value[len-1] == '\n' || clean_value[len-1] == '\r'))
			{
				clean_value[len-1] = '\0';
				len--;
			}
			set_env_var(env, name, clean_value);
		}
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
	}
}

void export_builtin(t_cmd *cmd, t_env **env)
{
	int i;
	
	if (!cmd->args[1])
	{
		// No arguments: print all environment variables
		print_exported_env(*env);
		return;
	}

	// Process each argument
	i = 1;
	while (cmd->args[i])
	{
		// Skip empty arguments (caused by quote parsing issues)
		if (cmd->args[i][0] == '\0')
		{
			i++;
			continue;
		}
		
		// Check if this argument ends with '=' and we need to reconstruct
		int len = ft_strlen(cmd->args[i]);
		if (len > 0 && cmd->args[i][len-1] == '=')
		{
			// Find the next non-empty argument to use as value
			int j = i + 1;
			while (cmd->args[j] && cmd->args[j][0] == '\0')
				j++; // Skip empty args
				
			if (cmd->args[j])
			{
				// Reconstruct: varname= + value
				char *full_assignment = malloc(len + ft_strlen(cmd->args[j]) + 1);
				if (full_assignment)
				{
					ft_strlcpy(full_assignment, cmd->args[i], len + 1);
					ft_strlcat(full_assignment, cmd->args[j], len + ft_strlen(cmd->args[j]) + 1);
					handle_export_var(full_assignment, env);
					i = j + 1; // Skip to after the value
					continue;
				}
			}
		}
		
		handle_export_var(cmd->args[i], env);
		i++;
	}
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
	
	char **filtered_env = malloc(sizeof(char *) * (count + 1));
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
			filtered_env[i] = malloc(name_len + value_len + 2);
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
