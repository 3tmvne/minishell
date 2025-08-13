#include "minishell.h"

void	pipes(t_pipeline *cmds, t_shell_state *shell)
{
	int		i;
	int		fd[2];
	int		prev_fd;
	pid_t	pid;
	char	**env_array;

	prev_fd = -1;
	env_array = env_to_array(shell->env);

	i = 0;
	prev_fd = -1;
	while (i < cmds->cmd_count)
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
			if (built_cmd(&cmds->commands[i], shell))
				exit(EXIT_SUCCESS);
			extern_cmd(&cmds->commands[i], env_array);
			exit(EXIT_FAILURE);
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
		i++;
	}
	// Wait for all children to finish
	while (wait(NULL) > 0); 
	
	// No free: memory is managed by GC or intentionally leaked
}
