/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:25:16 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 23:24:19 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_cmd(t_cmd *cmd, t_shell_state *shell, t_pipe_data *data)
{
	setup_child_process(data->prev_fd, data->fd, data->i, data->cmd_count);
	execute_child_command(cmd, shell);
}

void	init_pipes_vars(t_pipes_vars *vars, t_pipeline *cmds)
{
	vars->last_pid = -1;
	vars->cmds_list = cmds->commands;
	vars->pids = allocate_pids(cmds->cmd_count);
	vars->data.cmd_count = cmds->cmd_count;
	vars->data.fd = vars->fd;
	vars->data.i = 0;
	vars->prev_fd = -1;
}

void	process_single_command(t_pipes_vars *vars, t_shell_state *shell)
{
	vars->data.prev_fd = vars->prev_fd;
	vars->pids[vars->data.i] = create_child_process(vars->fd, vars->data.i,
			vars->data.cmd_count);
	if (vars->data.i == vars->data.cmd_count - 1)
		vars->last_pid = vars->pids[vars->data.i];
	if (vars->pids[vars->data.i] == 0)
		exe_cmd(vars->cmds_list, shell, &vars->data);
	else
		handle_parent_process(&vars->prev_fd, vars->fd, vars->data.i,
			vars->data.cmd_count);
	vars->cmds_list = vars->cmds_list->next;
	vars->data.i++;
}

void	pipes(t_pipeline *cmds, t_shell_state *shell)
{
	t_pipes_vars	vars;

	init_pipes_vars(&vars, cmds);
	while (vars.data.i < vars.data.cmd_count && vars.cmds_list)
	{
		process_single_command(&vars, shell);
	}
	wait_for_processes(vars.pids, cmds->cmd_count, vars.last_pid, shell);
}
