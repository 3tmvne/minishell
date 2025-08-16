#include "minishell.h"

void	pipes(t_pipeline *cmds, t_shell_state *shell)
{
	int		i;
	int		fd[2];
	int		prev_fd;
	pid_t	pid;

	i = 0;
	prev_fd = -1;
	t_cmd *cmds_list = cmds->commands;
	while (i < cmds->cmd_count || cmds_list)
	{
		//? Create pipe only if not the last command
		if (i < cmds->cmd_count - 1)
		{
			if (pipe(fd) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0) //* CHILD
		{
			//? Redirect stdin if not first command
			if (prev_fd != -1)
			{
				if (dup2(prev_fd, STDIN_FILENO) == -1)
				{
					perror("dup2");
					exit(EXIT_FAILURE);
				}
				close(prev_fd);
			}
			//? Redirect stdout if not last command
			if (i < cmds->cmd_count - 1)
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
				{
					perror("dup2");
					exit(EXIT_FAILURE);
				}
				close(fd[0]);
				close(fd[1]);
			}
			// Execute builtin or external command
			if (built_cmd(cmds_list, shell))
				exit(EXIT_SUCCESS);
			extern_cmd(cmds_list, shell);
		}
		else //* PARENT
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (i < cmds->cmd_count - 1)
			{
				close(fd[1]);    // Parent won't write
				prev_fd = fd[0]; // Keep read end for next child
			}
		}
		cmds_list = cmds_list->next; // Move to the next command
		i++;
	}
	// close(prev_fd); // Close the last read end
	// Wait for all children to finish
	while (wait(NULL) > 0)
		;
	// No free: memory is managed by GC or intentionally leaked
}
