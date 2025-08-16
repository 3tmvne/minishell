#include "minishell.h"

void pipes(t_pipeline *cmds, t_shell_state *shell)
{
int i, fd[2], prev_fd;
pid_t *pids;
if (!cmds || cmds->cmd_count <= 0) return;
pids = ft_malloc(sizeof(pid_t) * cmds->cmd_count);
prev_fd = -1; i = 0;
while (i < cmds->cmd_count) {
if (i < cmds->cmd_count - 1 && pipe(fd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }
pids[i] = fork();
if (pids[i] == -1) { perror("fork"); exit(EXIT_FAILURE); }
if (pids[i] == 0) {
signal(SIGPIPE, SIG_IGN);
if (prev_fd != -1) { dup2(prev_fd, STDIN_FILENO); close(prev_fd); }
if (i < cmds->cmd_count - 1) { dup2(fd[1], STDOUT_FILENO); close(fd[0]); close(fd[1]); }
if (built_cmd(&cmds->commands[i], shell)) exit(EXIT_SUCCESS);
extern_cmd(&cmds->commands[i], shell); exit(EXIT_FAILURE);
} else {
if (prev_fd != -1) close(prev_fd);
if (i < cmds->cmd_count - 1) { close(fd[1]); prev_fd = fd[0]; }
}
i++;
}
if (prev_fd != -1) close(prev_fd);
i = 0;
while (i < cmds->cmd_count) {
int status; waitpid(pids[i], &status, 0);
if (i == cmds->cmd_count - 1) {
if (WIFEXITED(status)) shell->last_exit_status = WEXITSTATUS(status);
else if (WIFSIGNALED(status)) shell->last_exit_status = 128 + WTERMSIG(status);
}
i++;
}
}
