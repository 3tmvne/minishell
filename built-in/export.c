/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:22:18 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/26 01:34:27 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_exported_env(t_env *env)
{
	char	**sorted_env;
	int		i;

	sorted_env = create_sorted_env_for_export(env);
	if (!sorted_env)
		return ;
	i = 0;
	while (sorted_env[i])
	{
		print_env_line(sorted_env[i]);
		i++;
	}
}

static int	handle_export_var(const char *var, t_env **env)
{
	char	*name;
	char	*equals_pos;
	char	*parsed_value;

	name = extract_var_name(var);
	if (!name || !is_valid_varname(name))
	{
		ft_putstr_fd("export: '", STDERR_FILENO);
		ft_putstr_fd((char *)var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (1);
	}
	equals_pos = ft_strchr(var, '=');
	if (equals_pos)
	{
		parsed_value = clean_value(equals_pos + 1);
		if (!parsed_value)
			return (1);
		set_env_var(env, name, parsed_value);
	}
	else
		add_new_var_to_list(env, name);
	return (0);
}

int	export_builtin(t_cmd *cmd, t_env **env)
{
	int	i;
	int	exit_status;
	int	result;

	exit_status = 0;
	if (!cmd->args[1])
	{
		print_exported_env(*env);
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (!cmd->args[i] || cmd->args[i][0] == '\0')
		{
			i++;
			continue ;
		}
		result = handle_export_var(cmd->args[i], env);
		if (result != 0)
			exit_status = result;
		i++;
	}
	return (exit_status);
}
