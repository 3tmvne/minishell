#include "minishell.h"

void	extern_cmd(t_cmd *cmd, char **env)
{
	int		i;
	char	**path;
	char	*fpath;

	i = 0;
	path = NULL;
	while (1)
	{
		if (strncmp(env[i], "PATH", 4))
		{
			path = ft_split(&env[i][5], ':');
			i = 0;
			while (path[i])
			{
				fpath = ft_strjoin(path[i], "/");
				if (!access(ft_strjoin(fpath, cmd->args[0]), F_OK))
				{
					execve(ft_strjoin(fpath, cmd->args[0]), cmd->args, env);
					perror("execve");
				}
				else if (!path[i])
				{
					ft_putstr_fd(cmd->args[0], 2);
					ft_putstr_fd(": command not found\n", 2);
				}
				i++;
			}
		}
		i++;
	}
}

int	built_cmd(t_cmd *cmd, char **env)
{
	if (!ft_strncmp(cmd->args[0], "echo", ft_strlen(cmd->args[0])))
		echo(cmd);
	else if (!ft_strncmp(cmd->args[0], "export", ft_strlen(cmd->args[0])))
		export_builtin(cmd, env);
	// else if (!ft_strncmp(cmd->args[0], "unset", ft_strlen(cmd->args[0])))
	// 	unset_builtin(cmd, env);
	else if (!ft_strncmp(cmd->args[0], "pwd", ft_strlen(cmd->args[0])))
		pwd_builtin();
	else if (!ft_strncmp(cmd->args[0], "env", ft_strlen(cmd->args[0])))
		env_builtin(env);
	else if (!ft_strncmp(cmd->args[0], "cd", ft_strlen(cmd->args[0])))
		cd(cmd, env);
	else if (!ft_strncmp(cmd->args[0], "exit", ft_strlen(cmd->args[0])))
		exit_builtin(cmd, 0);
	else
		return (0);
	return (1);
}

void	single_cmd(t_cmd *cmd, char **env)
{
	int	pid;

	pid = -1;
	if (!built_cmd(cmd, env))
		pid = fork();
	if (pid == 0)
		extern_cmd(cmd, env);
}

void	execute(t_pipeline *line, char **env)
{
	if (line->cmd_count == 1)
		single_cmd(line->commands, env);
	else
		pipes(line, env);
}