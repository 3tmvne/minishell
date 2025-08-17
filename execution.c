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
	*err = 127;    // Command not found exit status
	return (NULL); // Command not found
}

void	extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char	**env_array;
	char	*path;
	int		err;

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
	if (!ft_strcmp(cmd->args[0], "echo"))
		echo(cmd);
	else if (!ft_strcmp(cmd->args[0], "export"))
		shell->last_exit_status = export_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		shell->last_exit_status = unset_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "pwd"))
		pwd_builtin();
	else if (!ft_strcmp(cmd->args[0], "env"))
		env_builtin(shell->env);
	else if (!ft_strcmp(cmd->args[0], "cd"))
		cd(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "exit"))
		exit_builtin(cmd, shell->last_exit_status);
	else
		return (0);
	return (1);
}

void	single_cmd(t_cmd *cmd, t_shell_state *shell)
{
	int	pid = 0;
	int is_builtin = 1;
	
	if (cmd->args)
	{
		int fd0 = dup(0);
		int fd1 = dup(1);
		if (apply_redirection(cmd->redirections) == -1)
		{
			close(fd0);
			close(fd1);
		}
		if (!built_cmd(cmd, shell))
		{
			is_builtin = 0;
		}
		dup2(fd0, 0);
		dup2(fd1, 1);
		close(fd0);
		close(fd1);
	}
	if (!is_builtin)
	{
		pid = fork();
		if (pid == 0)
		{
			apply_redirection(cmd->redirections);
			extern_cmd(cmd, shell);
			exit(EXIT_FAILURE); // If execve fails, exit with failure
			//exit(EXIT_SUCCESS); // If built-in command executed successfully
		}
	}
	waitpid(pid, NULL, 0);
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
