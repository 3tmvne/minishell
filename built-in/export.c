/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 02:22:44 by REGRAGUI-A        #+#    #+#             */
/*   Updated: 2025/08/04 05:49:53 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (name[i] != '_' && !ft_isalnum(name[i]))
			return (0);
		i++;
	}
	return (1);
}

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

/**
 * Adds a new variable to the environment array
 */
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

/**
 * Prints environment variables in the export format: declare -x NAME="VALUE"
 */
static void	print_exported_env(char **env)
{
	char	**sorted_env;
	int		i;
	int		j;
	// int		has_value;

	sorted_env = create_sorted_env(env);
	if (!sorted_env)
		return ;
	i = -1;
	while (sorted_env[++i])
	{
		ft_putstr_fd("declare -x ", 1);
		j = 0;
		// has_value = 0;
		while (sorted_env[i][j] && sorted_env[i][j] != '=')
			write(1, &sorted_env[i][j++], 1);
		if (sorted_env[i][j] == '=')
		{
			// has_value = 1;
			write(1, "=", 1);
			write(1, "\"", 1);
			ft_putstr_fd(&sorted_env[i][j + 1], 1);
			write(1, "\"", 1);
		}
		write(1, "\n", 1);
	}
	// Free sorted environment
	i = -1;
	while (sorted_env[++i])
		free(sorted_env[i]);
	free(sorted_env);
}

/**
 * Extract variable name from a string (before '=' character)
 */
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

/**
 * Process a single export variable
 * Returns updated environment array
 */
static char **process_export_var(char **env, char *var)
{
	char	*name;
	char	*value;
	int		has_equals;
	int		i;

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
	value = get_env_value(name, env);
	if (value)
	{
		// Variable exists
		if (has_equals)
		{
			// Update existing variable
			update_env_value(name, var + i + 1, env);
		}
		// Otherwise just mark it for export (which is already done)
		// ❌ NE PAS FAIRE: free(value); car get_env_value ne fait pas malloc
	}
	else if (has_equals)
	{
		// Variable doesn't exist, add itt
		env = add_to_env_array(env, var);
	}
	else
	{
		// Just name without value, add empty var
		char *empty_var = create_env_string(name, "");
		env = add_to_env_array(env, empty_var);
		free(empty_var);
	}
	
	free(name);
	return (env);
}

/**
 * Implements the export builtin command
 * Adds variables to the environment or displays all variables
 */
char **export_builtin(t_token *tokens, char **env)
{
	t_token	*current;
	int		has_args;
	char	*var_string;
	int		total_len;
	
	has_args = 0;
	current = tokens->next; // Skip the "export" token
	
	// Parcourir TOUS les tokens pour reconstruire les arguments
	while (current)
	{
		if (current->type == WS)
		{
			// Ignorer les espaces
			current = current->next;
			continue;
		}
		else if (current->type == WORD)
		{
			has_args = 1;
			
			// Reconstruire la variable complète en combinant les tokens
			var_string = ft_strdup(current->value);
			current = current->next;
			
			// Si le token suivant n'est pas un espace, on continue à concatener
			while (current && current->type != WS)
			{
				total_len = ft_strlen(var_string) + ft_strlen(current->value) + 1;
				char *temp = malloc(total_len);
				if (!temp)
					break;
				ft_strlcpy(temp, var_string, total_len);
				ft_strlcat(temp, current->value, total_len);
				free(var_string);
				var_string = temp;
				current = current->next;
			}
			
			// Maintenant on a la variable complète, la traiter
			if (!is_valid_varname(var_string))
			{
				ft_putstr_fd("minishell: export: '", 2);
				ft_putstr_fd(var_string, 2);
				ft_putstr_fd("': not a valid identifier\n", 2);
			}
			else
			{
				env = process_export_var(env, var_string);
			}
			
			free(var_string);
			continue; // On a déjà avancé current dans la boucle interne
		}
		else
		{
			current = current->next;
		}
	}
	
	// If no arguments, print all variables
	if (!has_args)
		print_exported_env(env);
		
	return (env);
}