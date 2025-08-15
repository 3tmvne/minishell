#include "minishell.h"

char	*find_command_path(const char *cmd, t_env *env)
{
	char	*path;
	char	**paths;
	int		i;
	char	*full_path;

	path = get_env_value_list(env, "PATH");
	if (!path)
		return (NULL); // PATH not set
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL); // Memory allocation failed
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			return (full_path);
		}
		i++;
	}
	return (NULL); // Command not found
}

void	extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char	**env_array;
	char	*path;

	// Prepare the environment for execve
	env_array = env_to_array(shell->env);

	if (!env_array)
		return ;
	path = find_command_path(cmd->args[0], shell->env);
	if( !path)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": Command not found", STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		shell->last_exit_status = 127; // Command not found exit status
		return ;
	}
	// Execute the command
	if (execve(path, cmd->args, env_array) == -1)
		perror("execve");
	// Free the environment array
}

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!ft_strncmp(cmd->args[0], "echo", ft_strlen(cmd->args[0])))
		echo(cmd);
	else if (!ft_strncmp(cmd->args[0], "export", ft_strlen(cmd->args[0])))
		shell->last_exit_status = export_builtin(cmd, &shell->env);
	else if (!ft_strncmp(cmd->args[0], "unset", ft_strlen(cmd->args[0])))
		shell->last_exit_status = unset_builtin(cmd, &shell->env);
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

	pid = -1;
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (!built_cmd(cmd, shell))
	{
		pid = fork();
	}
	if (pid == 0)
	{
		extern_cmd(cmd, shell);
		exit(1);
		// Le processus enfant se termine ici, pas besoin de free
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
	// No free: memory is managed by GC or intentionally leaked
}

void	execute(t_pipeline *line, t_shell_state *shell)
{
	if (!line || !shell || !line->commands)
		return ;
	if (line->cmd_count == 1)
		single_cmd(line->commands, shell);
	else
		pipes(line, shell);
}