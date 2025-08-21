/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:10:31 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 21:12:14 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*create_env_string(const char *name, const char *value)
{
	char	*result;
	int		value_len;
	int		name_len;
	int		i;
	int		j;

	if (!name || !value)
		return (NULL);
	name_len = ft_strlen(name);
	value_len = ft_strlen(value);
	result = ft_malloc(name_len + value_len + 2);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (name[i])
		result[j++] = name[i++];
	result[j++] = '=';
	i = 0;
	while (value[i])
		result[j++] = value[i++];
	result[j] = '\0';
	return (result);
}

t_env	*create_env_node(const char *name, const char *value)
{
	t_env	*node;

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

t_env	*find_env_var(t_env *env, const char *name)
{
	t_env	*current;

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

char	*get_env_value_list(t_env *env, const char *name)
{
	t_env	*var;

	var = find_env_var(env, name);
	if (var)
		return (var->value);
	return (NULL);
}

void	set_env_var(t_env **env, const char *name, const char *value)
{
	t_env	*var;
	t_env	*new_node;

	if (!env || !name || !value)
	{
		return ;
	}
	var = find_env_var(*env, name);
	if (var)
	{
		var->value = ft_strdup(value);
	}
	else
	{
		new_node = create_env_node(name, value);
		if (new_node)
		{
			new_node->next = *env;
			*env = new_node;
		}
	}
}

void	unset_env_var(t_env **env, const char *name)
{
	t_env	*current;
	t_env	*prev;

	if (!env || !*env || !name)
		return ;
	current = *env;
	prev = NULL;
	while (current)
	{
		if (strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
			free(current->name);
			if (current->value)
				free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

char	**env_to_array(t_env *env)
{
	t_env	*current;
	char	**array;
	int		count;
	int		i;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
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

t_env	*array_to_env_list(char **env)
{
	t_env	*env_list;
	char	*equals_pos;
	char	*name;
	char	*value;
	int		i;

	if (!env)
		return (NULL);
	env_list = NULL;
	i = 0;
	while (env[i])
	{
		equals_pos = ft_strchr(env[i], '=');
		if (equals_pos)
		{
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
