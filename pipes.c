#include "minishell.h"

void	pipes(t_pipeline *cmds, t_shell_state *shell)
{
	int		i;
	int		fd[2];
	int		prev_fd;
	pid_t	last_pid = -1; // Store PID of last command
	pid_t	*pids; // Array to store all PIDs
	int		status;
	int		last_status = 0; // Status of the last command specifically
	t_cmd	*cmds_list;

	i = 0;
	prev_fd = -1;
	cmds_list = cmds->commands;
	
	// Allocate array to store all PIDs
	pids = ft_malloc(sizeof(pid_t) * cmds->cmd_count);
	if (!pids)
	{
		perror("ft_malloc");
		exit(EXIT_FAILURE);
	}

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
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		
		// Store PID of the last command in pipeline
		if (i == cmds->cmd_count - 1)
			last_pid = pids[i];

		signal(SIGINT, SIG_IGN);
		if (pids[i] == 0) //* CHILD
		{
			set_child_signals();
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
			if (is_built_cmd(cmds_list))
			{
				if (cmds_list->redirections)
				{
					if (redirection(cmds_list))
						return ;
				}
				built_cmd(cmds_list, shell);
				if (cmds_list->redirections)
					restor_fd(cmds_list);
				exit(0);
			}
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
	
	// Wait for all processes and get the status of the LAST command specifically
	i = 0;
	int was_interrupted = 0; // Track if any process was interrupted
	while (i < cmds->cmd_count)
	{
		pid_t waited_pid = waitpid(pids[i], &status, 0);
		if (waited_pid == -1)
		{
			perror("waitpid");
			break;
		}
		
		// Check if any process was interrupted by SIGINT
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			was_interrupted = 1;
		
		// If this is the last command, save its status
		if (pids[i] == last_pid)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				int sig = WTERMSIG(status);
				last_status = 128 + sig;
			}
		}
		i++;
	}
	
	// Update shell exit status with the last command's status
	shell->last_exit_status = last_status;
	
	// Handle signal output messages - add newline if pipeline was interrupted
	if (was_interrupted)
		write(1, "\n", 1); // Add newline after ^C
	else if (shell->last_exit_status == 131)
		write(1, "Quit (core dumped)\n", 19);
		
	handle_signals();
	// No free: memory is managed by GC or intentionally leaked
}
