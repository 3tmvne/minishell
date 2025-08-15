#include "minishell.h"

/**
 * Créer un nouvel élément d'environnement
 */
t_env *create_env_node(const char *name, const char *value)
{
	t_env *node;

	if (!name)
		return (NULL);
	
	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	
	node->name = ft_strdup(name);
	node->value = value ? ft_strdup(value) : NULL;
	node->next = NULL;
	
	if (!node->name || (value && !node->value))
	{
		return (NULL);
	}
	
	return (node);
}

/**
 * Chercher une variable dans la liste d'environnement
 */
t_env *find_env_var(t_env *env, const char *name)
{
	t_env *current;

	if (!name)
		return (NULL);
	
	current = env;
	while (current)
	{
		if (strcmp(current->name, name) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

/**
 * Récupérer la valeur d'une variable d'environnement
 */
char *get_env_value_list(t_env *env, const char *name)
{
	t_env *var;

	var = find_env_var(env, name);
	if (var)
		return (var->value);
	return (NULL);
}

/**
 * Ajouter ou modifier une variable d'environnement
 */
void set_env_var(t_env **env, const char *name, const char *value)
{
	t_env *var;
	t_env *new_node;

	if (!env || !name || !value)
	{
		return;
	}
	
	var = find_env_var(*env, name);
	if (var)
	{
		// Variable existe, mettre à jour la valeur
		var->value = ft_strdup(value);
	}
	else
	{
		// Variable n'existe pas, l'ajouter au début de la liste
		new_node = create_env_node(name, value);
		if (new_node)
		{
			new_node->next = *env;
			*env = new_node;
		}
	}
}

/**
 * Supprimer une variable d'environnement
 */
void unset_env_var(t_env **env, const char *name)
{
	t_env *current;
	t_env *prev;

	if (!env || !*env || !name)
		return;
	
	current = *env;
	prev = NULL;
	
	while (current)
	{
		if (strcmp(current->name, name) == 0)
		{
			// Update the linked list pointers
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
				
			// Free the memory of the removed node
			free(current->name);
			if (current->value)
				free(current->value);
			free(current);
			
			return;
		}
		prev = current;
		current = current->next;
	}
}

/**
 * Convertir la liste d'environnement en tableau pour execve()
 */
char **env_to_array(t_env *env)
{
	t_env *current;
	char **array;
	int count;
	int i;

	// Compter le nombre de variables
	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	
	// Allouer le tableau
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	
	// Remplir le tableau
	i = 0;
	current = env;
	while (current)
	{
		array[i] = create_env_string(current->name, current->value);
		if (!array[i])
		{
			return (NULL);
		}
		current = current->next;
		i++;
	}
	array[i] = NULL;
	
	return (array);
}

/**
 * Convertir un tableau d'environnement en liste chaînée
 */
t_env *array_to_env_list(char **env)
{
	t_env *env_list;
	char *equals_pos;
	char *name;
	char *value;
	int i;

	if (!env)
		return (NULL);
	
	env_list = NULL;
	i = 0;
	
	while (env[i])
	{
		equals_pos = ft_strchr(env[i], '=');
		if (equals_pos)
		{
			// Extraire le nom et la valeur
			name = ft_substr(env[i], 0, equals_pos - env[i]);
			value = ft_strdup(equals_pos + 1);
			
			if (name && value)
			{
				set_env_var(&env_list, name, value);
			}
		}
		i++;
	}	
	return (env_list);
}
