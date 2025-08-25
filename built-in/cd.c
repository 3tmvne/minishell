/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:05:35 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 22:54:43 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_env_value(const char *name, char **env)
{
	int	i;
	int	name_len;

	if (!name || !env)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (&env[i][name_len + 1]);
		i++;
	}
	return (NULL);
}

void	update_env_value(const char *name, const char *value, char **env)
{
	int		i;
	int		name_len;
	char	*new_var;

	if (!name || !value || !env)
		return ;
	name_len = ft_strlen(name);
	new_var = create_env_string(name, value);
	if (!new_var)
		return ;
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
		{
			env[i] = new_var;
			return ;
		}
		i++;
	}
	env[i] = new_var;
	env[i + 1] = NULL;
}

static int	cd_change_directory(const char *path, t_env **env, char *oldpwd)
{
	char	*newpwd;

	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	newpwd = getcwd(NULL, 0);
	if (newpwd)
	{
		add_to_gc(newpwd);
		set_env_var(env, "PWD", newpwd);
		set_env_var(env, "OLDPWD", oldpwd);
	}
	else
	{
		ft_putstr_fd("cd: error updating PWD\n", 2);
	}
	return (0);
}

static char	*cd_get_target_path(t_cmd *cmd, t_env **env)
{
	char	*path;

	if (!cmd->args[1])
	{
		path = get_env_value_list(*env, "HOME");
		if (!path)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (NULL);
		}
	}
	else
		path = cmd->args[1];
	return (path);
}

int	cd(t_cmd *cmd, t_env **env)
{
	char	*path;
	char	*oldpwd;

	if (cmd && cmd->args && cmd->args[1] && cmd->args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
	{
		perror("getcwd failed");
		return (1);
	}
	add_to_gc(oldpwd);
	path = cd_get_target_path(cmd, env);
	if (!path)
		return (1);
	return (cd_change_directory(path, env, oldpwd));
}
