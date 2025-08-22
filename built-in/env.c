/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:45:35 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/22 16:49:38 by aregragu         ###   ########.fr       */
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

void	env_builtin(t_env *env)
{
	t_env	*current;

	if (!env)
		return ;
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
}
