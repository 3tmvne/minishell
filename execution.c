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

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	if (!ft_strncmp(cmd->args[0], "echo", ft_strlen(cmd->args[0])))
		echo(cmd);
	else if (!ft_strncmp(cmd->args[0], "export", ft_strlen(cmd->args[0])))
		export_builtin(cmd, &shell->env);
	else if (!ft_strncmp(cmd->args[0], "unset", ft_strlen(cmd->args[0])))
		unset_builtin(cmd, &shell->env);
	else if (!ft_strncmp(cmd->args[0], "pwd", ft_strlen(cmd->args[0])))
		pwd_builtin();
	else if (!ft_strncmp(cmd->args[0], "env", ft_strlen(cmd->args[0])))
		env_builtin(shell->env);
	else if (!ft_strncmp(cmd->args[0], "cd", ft_strlen(cmd->args[0])))
		cd(cmd, &shell->env);
	else if (!ft_strncmp(cmd->args[0], "exit", ft_strlen(cmd->args[0])))
		exit_builtin(cmd, shell->last_exit_status);
	else
		return (0);
	return (1);
}

void	single_cmd(t_cmd *cmd, t_shell_state *shell)
{
	int	pid;
	char **env_array;

	pid = -1;
	if (!built_cmd(cmd, shell))
	{
		pid = fork();
		env_array = env_to_array(shell->env);
	}
	if (pid == 0)
	{
		extern_cmd(cmd, env_array);
		// Le processus enfant se termine ici, pas besoin de free
	}
	// Le processus parent libère env_array
	if (pid != -1 && env_array)
	{
		int i = 0;
		while (env_array[i])
		{
			free(env_array[i]);
			i++;
		}
		free(env_array);
	}
}

void	execute(t_pipeline *line, t_shell_state *shell)
{
	if (line->cmd_count == 1)
		single_cmd(line->commands, shell);
	else
		pipes(line, shell);
}