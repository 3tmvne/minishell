#include "minishell.h"

char	*find_command_path(const char *cmd, t_env *env, int *err)
{
	char	*path;
	char	**paths;
	int		i;
	char	*full_path;

	*err = 0; // Initialize error code to 0
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
		if (access(full_path, X_OK) != 0 && access(full_path, F_OK) == 0)
		{
			*err = 126; // Permission denied exit status
			return (full_path);
		}
		if (access(full_path, X_OK) == 0)
			return (full_path);
		i++;
	}
	*err = 127; // Command not found exit status
	return (NULL); // Command not found
}

void	extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char	**env_array;
	char	*path;
	int		err;

	// Prepare the environment for execve
	env_array = env_to_array(shell->env);
	if (!env_array)
		exit(EXIT_FAILURE);
	
	path = find_command_path(cmd->args[0], shell->env, &err);
	if (err == 127)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": Command not found", STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		exit(127); // Exit with command not found status
	}
	else if (err == 126)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": Permission denied", STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		exit(126); // Exit with permission denied status
	}
	// Execute the command
	if (execve(path, cmd->args, env_array) == -1)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
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
	if (cmd->redirections)
	{
		pid = fork();
		if (pid == 0)
		{
			if (apply_redirection(cmd->redirections) == -1)
				exit(EXIT_FAILURE);
			if (!built_cmd(cmd, shell))
				extern_cmd(cmd, shell);
		}
	}
	else if (!built_cmd(cmd, shell))
		pid = fork();
	if (pid == 0)
	{
		extern_cmd(cmd, shell);
		exit(1);
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
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
