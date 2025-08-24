/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:33:29 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/24 21:46:35 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	wait_single_process(pid_t *pids, int i, pid_t last_pid, int *status)
{
	pid_t	waited_pid;
	int		last_status;

	last_status = 0;
	waited_pid = waitpid(pids[i], status, 0);
	if (waited_pid == -1)
	{
		perror("waitpid");
		return (-1);
	}
	if (pids[i] == last_pid)
	{
		if (WIFEXITED(*status))
			last_status = WEXITSTATUS(*status);
		else if (WIFSIGNALED(*status))
			last_status = 128 + WTERMSIG(*status);
	}
	return (last_status);
}

void	wait_for_processes(pid_t *pids, int cmd_count, pid_t last_pid,
		t_shell_state *shell)
{
	int	i;
	int	status;
	int	last_status;
	int	was_interrupted;
	int	result;

	i = 0;
	last_status = 0;
	was_interrupted = 0;
	while (i < cmd_count)
	{
		result = wait_single_process(pids, i, last_pid, &status);
		if (result == -1)
			break ;
		if (result > 0)
			last_status = result;
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			was_interrupted = 1;
		i++;
	}
	shell->last_exit_status = last_status;
	handle_signal_output(was_interrupted, last_status);
}

pid_t	create_child_process(int *fd, int i, int cmd_count)
{
	pid_t	pid;

	if (i < cmd_count - 1)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			free_gc_all();
			exit(EXIT_FAILURE);
		}
	}
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free_gc_all();
		exit(EXIT_FAILURE);
	}
	return (pid);
}
