/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:45:35 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 13:06:52 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	env_builtin(t_cmd *cmd, t_env *env)
{
	t_env	*current;

	if (cmd->args[1])
	{
		ft_putstr_fd("env: too many arguments\n", 2);
		return (1);
	}
	if (!env)
		return (0);
	current = env;
	while (current)
	{
		if (current->value)
		{
			ft_putstr_fd(current->name, 1);
			if (current->value)
			{
				write(1, "=", 1);
				ft_putstr_fd(current->value, 1);
			}
			write(1, "\n", 1);
		}
		current = current->next;
	}
	return (0);
}
