/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:26:04 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/22 18:07:02 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	*allocate_pids(int cmd_count)
{
	pid_t	*pids;

	pids = ft_malloc(sizeof(pid_t) * cmd_count);
	return (pids);
}

void	setup_child_process(int prev_fd, int *fd, int i, int cmd_count)
{
	set_child_signals();
	if (prev_fd != -1)
	{
		if (dup2(prev_fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		close(prev_fd);
	}
	if (i < cmd_count - 1)
	{
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		close(fd[0]);
		close(fd[1]);
	}
}

void	execute_child_command(t_cmd *cmd, t_shell_state *shell)
{
	if (is_built_cmd(cmd))
	{
		if (cmd->redirections)
		{
			if (redirection(cmd))
				exit(1);
		}
		built_cmd(cmd, shell);
		if (cmd->redirections)
			restor_fd(cmd);
		exit(0);
	}
	extern_cmd(cmd, shell);
}

void	handle_parent_process(int *prev_fd, int *fd, int i, int cmd_count)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (i < cmd_count - 1)
	{
		close(fd[1]);
		*prev_fd = fd[0];
	}
}

void	handle_signal_output(int was_interrupted, int last_status)
{
	if (was_interrupted)
		write(1, "\n", 1);
	else if (last_status == 131)
		write(1, "Quit (core dumped)\n", 19);
	handle_signals();
}
