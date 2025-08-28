/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 17:32:18 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/26 01:34:16 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_env_nodes(t_env *env)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

char	**create_sorted_env_for_export(t_env *env)
{
	int		count;
	char	**array;
	t_env	*current;
	int		i;

	count = count_env_nodes(env);
	array = ft_malloc(sizeof(char *) * (count + 1));
	current = env;
	i = 0;
	if (!array)
		return (NULL);
	while (current)
	{
		array[i] = create_env_string(current->name, current->value);
		if (!array[i])
			return (NULL);
		current = current->next;
		i++;
	}
	array[i] = NULL;
	return (array);
}

void	print_env_line(char *env_str)
{
	char	*eq;
	int		name_len;

	eq = ft_strchr(env_str, '=');
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	if (eq)
	{
		name_len = eq - env_str;
		write(STDOUT_FILENO, env_str, name_len);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(eq + 1, STDOUT_FILENO);
		ft_putstr_fd("\"\n", STDOUT_FILENO);
	}
	else
	{
		ft_putstr_fd(env_str, STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
	}
}

int	count_vars_with_value(t_env *env)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
	return (count);
}

char	*create_filtered_env_string(t_env *current)
{
	int		name_len;
	int		value_len;
	char	*result;

	name_len = ft_strlen(current->name);
	value_len = ft_strlen(current->value);
	result = ft_malloc(name_len + value_len + 2);
	if (!result)
		return (NULL);
	ft_strlcpy(result, current->name, name_len + 1);
	ft_strlcat(result, "=", name_len + 2);
	ft_strlcat(result, current->value, name_len + value_len + 2);
	return (result);
}
