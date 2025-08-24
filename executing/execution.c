/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:32:01 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/24 22:10:18 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!ft_strcmp(cmd->args[0], "echo"))
	{
		echo(cmd);
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "export"))
		shell->last_exit_status = export_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		shell->last_exit_status = unset_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "pwd"))
	{
		pwd_builtin();
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "env"))
		shell->last_exit_status = env_builtin(cmd, shell->env);
	else if (!ft_strcmp(cmd->args[0], "cd"))
		shell->last_exit_status = cd(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "exit"))
		exit_builtin(cmd, shell->last_exit_status);
	return (1);
}

void	extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char	**env_array;
	char	*path;
	int		err;

	// if (!cmd || !cmd->args || !cmd->args[0] || !shell)
	// {
	// 	free_gc_all();
	// 	exit(0);
	// }
	env_array = env_to_array(shell->env);
	if (cmd->redirections)
		if (redirection(cmd))
		{
			free_gc_all();	
			exit(1);
		}
	if (!env_array)
	{
		free_gc_all();
		exit(EXIT_FAILURE);
	}
	path = find_command_path(cmd->args[0], shell->env, &err);
	if (!path || err != 0)
		print_and_exit_external_error(cmd->args[0], err);
	if (execve(path, cmd->args, env_array) == -1)
	{
		free_gc_all();
		perror("execve");
		exit(EXIT_FAILURE);
	}
}

int	is_built_cmd(t_cmd *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!ft_strcmp(cmd->args[0], "echo") || !ft_strcmp(cmd->args[0], "cd")
		|| !ft_strcmp(cmd->args[0], "unset") || !ft_strcmp(cmd->args[0], "env")
		|| !ft_strcmp(cmd->args[0], "export") || !ft_strcmp(cmd->args[0],
			"exit") || !ft_strcmp(cmd->args[0], "pwd"))
		return (1);
	return (0);
}

void	execute(t_pipeline *line, t_shell_state *shell)
{
	if (!line || !shell)
		return ;
	if (!line->commands->args || !line->commands->args[0])
	{
		if (line->commands->redirections)
		{
			if (redirection(line->commands))
				return ;
			restor_fd(line->commands);
		}
	}
	else if (line->commands->args[0][0] == '\0')
	{
		write(2, "minishell: : command not found\n", 32);
		shell->last_exit_status = 127;
		return ;
	}
	if (is_built_cmd(line->commands) && line->cmd_count == 1)
	{
		if (line->commands->redirections)
			if (redirection(line->commands))
				return ;
		built_cmd(line->commands, shell);
		if (line->commands->redirections)
			restor_fd(line->commands);
	}
	else
	{
		if (!line->commands || !line->commands->args || !line->commands->args[0] || !shell)
			return ;
		pipes(line, shell);
	}
}
