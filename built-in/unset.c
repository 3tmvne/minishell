/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:23:23 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 21:56:04 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Checks if a variable name is valid
 * Valid names must start with a letter or underscore
 * and can only contain alphanumeric characters and underscores
 */
int	is_valid_varname(const char *name)
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
			return ;
		}
		prev = current;
		current = current->next;
	}
}
/**
 * Implements the unset builtin command
 * Removes variables from the environment linked list structure
 * Returns the exit status (0 for success, 1 for error)
 */

int	unset_builtin(t_cmd *cmd, t_env **env)
{
	int	i;
	int	exit_status;

	if (!cmd || !cmd->args || !env)
		return (1);
	if (!cmd->args[1])
		return (0);
	i = 1;
	exit_status = 0;
	while (cmd->args[i])
	{
		if (is_valid_varname(cmd->args[i]))
		{
			unset_env_var(env, cmd->args[i]);
		}
		else
		{
			ft_putstr_fd("unset: '", STDERR_FILENO);
			ft_putstr_fd(cmd->args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			exit_status = 1;
		}
		i++;
	}
	return (exit_status);
}
