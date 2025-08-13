#include "minishell.h"

void	extern_cmd(t_cmd *cmd, char **env)
{

}

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
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

	if (!cmd || !cmd->args || !cmd->args[0])
		return;
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
	// No free: memory is managed by GC or intentionally leaked
}

void	execute(t_pipeline *line, t_shell_state *shell)
{
	if(!line || !shell || !line->commands)
		return;
	if (line->cmd_count == 1)
		single_cmd(line->commands, shell);
	else
		pipes(line, shell);
}