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
				while (--i >= 0)
					free(array[i]);
				free(array);
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
				while (--i >= 0)
					free(array[i]);
				free(array);
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
		return;
	
	int i = 0;
	while (sorted_env[i])
	{
		ft_putstr_fd("declare -x ", 1);
		int j = 0;
		
		// Print variable name
		while (sorted_env[i][j] && sorted_env[i][j] != '=')
			write(1, &sorted_env[i][j++], 1);
		
		// Print value if it exists
		if (sorted_env[i][j] == '=')
		{
			write(1, "=\"", 2);
			ft_putstr_fd(&sorted_env[i][j + 1], 1);
			write(1, "\"", 1);
		}
		// Sinon c'est une variable export-only, on ne print rien d'autre
		
		write(1, "\n", 1);
		free(sorted_env[i]);
		i++;
	}
	free(sorted_env);
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
	if (!name)
		return;
	
	if (!is_valid_varname(name))
	{
		printf("export: '%s': not a valid identifier\n", var);
		free(name);
		return;
	}
	
	equals_pos = ft_strchr(var, '=');
	if (equals_pos)
	{
		// Variable has a value (could be empty: VAR=)
		set_env_var(env, name, equals_pos + 1);
	}
	else
	{
		// Variable without value, just mark for export
		// Dans bash: export VAR (sans =) marque pour export mais pas dans env
		t_env *existing = find_env_var(*env, name);
		if (!existing)
		{
			// Créer nouvelle variable export-only (value = NULL)
			t_env *new_var = create_env_node(name, NULL);
			if (new_var)
			{
				new_var->next = *env;
				*env = new_var;
			}
		}
		// Si la variable existe déjà, ne rien faire (garde sa valeur actuelle)
	}
	
	free(name);
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
