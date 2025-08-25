/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 17:32:21 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 22:25:04 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_varname_export(const char *name)
{
	int	i;

	if (!name || !*name || name[0] == '=' || ft_isdigit(name[0]))
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

char	*extract_var_name(const char *var)
{
	int	i;

	i = 0;
	while (var[i] && var[i] != '=')
		i++;
	return (ft_substr(var, 0, i));
}

char	*clean_value(char *value)
{
	char	*parsed_value;
	int		len;

	parsed_value = ft_strdup(value);
	if (!parsed_value)
		return (NULL);
	len = ft_strlen(parsed_value);
	while (len > 0 && (parsed_value[len - 1] == '\n'
			|| parsed_value[len - 1] == '\r'))
	{
		parsed_value[len - 1] = '\0';
		len--;
	}
	return (parsed_value);
}

void	add_new_var_to_list(t_env **env, const char *name)
{
	t_env	*existing;
	t_env	*new_var;
	t_env	*cur;

	existing = find_env_var(*env, name);
	if (!existing)
	{
		new_var = create_env_node(name, NULL);
		if (new_var)
		{
			cur = *env;
			if (!cur)
				*env = new_var;
			else
			{
				while (cur->next)
					cur = cur->next;
				cur->next = new_var;
			}
		}
	}
}

char	**get_filtered_env_list(t_env *env)
{
	int		count;
	char	**filtered_env;
	t_env	*current;
	int		i;

	count = count_vars_with_value(env);
	filtered_env = ft_malloc(sizeof(char *) * (count + 1));
	current = env;
	i = 0;
	if (!filtered_env)
		return (NULL);
	while (current)
	{
		if (current->value)
		{
			filtered_env[i] = create_filtered_env_string(current);
			if (filtered_env[i])
				i++;
		}
		current = current->next;
	}
	filtered_env[i] = NULL;
	return (filtered_env);
}
