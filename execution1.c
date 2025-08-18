#include "minishell.h"

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	   if (!cmd || !cmd->args || !cmd->args[0])
			   return (0);
	if (!ft_strcmp(cmd->args[0], "echo")) {
		echo(cmd);
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "export"))
		shell->last_exit_status = export_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		shell->last_exit_status = unset_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "pwd")) {
		pwd_builtin();
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "env")) {
		env_builtin(shell->env);
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "cd"))
		cd(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "exit"))
		exit_builtin(cmd, shell->last_exit_status);
	else
		return (0);
	return (1);
}
// Helper: affiche une erreur de commande externe et quitte le processus fils




char	*find_command_path(const char *cmd, t_env *env, int *err)
{
	char    *path;
	char    **paths;
	int     i;
	char    *full_path;

	*err = 0; // Initialize error code to 0
	// Si le nom de commande contient un '/', tester directement ce chemin
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		if (access(cmd, F_OK) == 0)
		{
			*err = 126;
			return (ft_strdup(cmd));
		}
		*err = 127;
		return (NULL);
	}
	path = get_env_value_list(env, "PATH");
	if (!path)
	{
		// Si PATH n'est pas défini, tester ./cmd
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		if (access(cmd, F_OK) == 0)
		{
			*err = 126;
			return (ft_strdup(cmd));
		}
		*err = 127;
		return (NULL);
	}
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


static void print_and_exit_external_error(const char *cmd, int err)
{
	   if (err == 127)
	   {
			   ft_putstr_fd("minishell: ", STDERR_FILENO);
			   ft_putstr_fd((char *)cmd, STDERR_FILENO);
			   ft_putstr_fd(": command not found\n", STDERR_FILENO);
			   exit(127);
	   }
	   if (err == 126)
	   {
			   ft_putstr_fd((char *)cmd, STDERR_FILENO);
			   ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
			   exit(126);
	   }
}

void extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char    **env_array;
	char    *path;
	int     err;

	env_array = env_to_array(shell->env);
	if (!env_array)
		exit(EXIT_FAILURE);
	path = find_command_path(cmd->args[0], shell->env, &err);
	if (!path || err == 127 || err == 126)
		print_and_exit_external_error(cmd->args[0], err);
	if (execve(path, cmd->args, env_array) == -1)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
}


static void exec_external_with_status(t_cmd *cmd, t_shell_state *shell)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		apply_redirection(cmd->redirections);
		extern_cmd(cmd, shell);
		exit(EXIT_FAILURE);
	}
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->last_exit_status = WEXITSTATUS(status);
	// else if (WIFSIGNALED(status))
	// 	shell->last_exit_status = 128 + WTERMSIG(status);
}

void	single_cmd(t_cmd *cmd, t_shell_state *shell)
{
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
		// Si c'est exit, on quitte immédiatement sans forker
		if (cmd->args[0] && !ft_strcmp(cmd->args[0], "exit"))
		{
			exit_builtin(cmd, shell->last_exit_status);
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
		exec_external_with_status(cmd, shell);
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