/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sherch_path.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:33:24 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:36:08 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*check_absolute_path(const char *cmd, int *err)
{
	struct stat	st;

	if (access(cmd, F_OK) != 0)
	{
		*err = 1;
		return (NULL);
	}
	if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
	{
		*err = 2;
		return (NULL);
	}
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	*err = 126;
	return (NULL);
}

static char	*search_in_path(const char *cmd, char **paths, int *err)
{
	int		i;
	char	*full_path;
	char	*temp_path;

	i = 0;
	while (paths[i])
	{
		if (!ft_strcmp(cmd, ".") || !ft_strcmp(cmd, ".."))
			break ;
		temp_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp_path, cmd);
		if (access(full_path, X_OK) != 0 && access(full_path, F_OK) == 0)
		{
			*err = 126;
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
			return (full_path);
		i++;
	}
	*err = 127;
	return (NULL);
}

char	*find_command_path(const char *cmd, t_env *env, int *err)
{
	char	*path;
	char	**paths;

	*err = 0;
	if (ft_strchr(cmd, '/'))
		return (check_absolute_path(cmd, err));
	if (cmd[0] == '\0')
	{
		*err = 127;
		return (NULL);
	}
	path = get_env_value_list(env, "PATH");
	if (!path)
	{
		*err = 127;
		return (NULL);
	}
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(cmd, paths, err));
}

void	print_error_and_exit(const char *cmd, const char *error_msg,
		int exit_code)
{
	char	*tmp;
	char	*msg;

	tmp = ft_strjoin("minishell: ", cmd);
	msg = ft_strjoin(tmp, error_msg);
	write(2, msg, ft_strlen(msg));
	free_gc_all();
	exit(exit_code);
}

void	print_and_exit_external_error(const char *cmd, int err)
{
	if (err == 127)
		print_error_and_exit(cmd, ": command not found\n", 127);
	if (err == 126)
		print_error_and_exit(cmd, ": Permission denied\n", 126);
	if (err == 2)
		print_error_and_exit(cmd, ": Is a directory\n", 126);
	if (err == 1)
		print_error_and_exit(cmd, ": No such file or directory\n", 127);
}
