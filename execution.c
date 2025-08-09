#include "minishell.h"

int built_cmd(t_cmd *cmd, char **env)
{
    if(!ft_strncmp(cmd->args[0], "echo", 4))
        echo(cmd);
    else if(!ft_strncmp(cmd->args[0], "export", 6))
        export(cmd);
    else if(!ft_strncmp(cmd->args[0], "unset", 5))
        unset(cmd);
    else if(!ft_strncmp(cmd->args[0], "pwd", 3))
        pwd(cmd);
    else if(!ft_strncmp(cmd->args[0], "env", 3))
        env_builtin(cmd);
    else if(!ft_strncmp(cmd->args[0], "cd", 2))
        cd(cmd, env);
    else if(!ft_strncmp(cmd->args[0], "exit", 4))
        exit(cmd);
    else
        return 1;
    return 0;

}

void    single_cmd(t_cmd *cmd, char **env)
{
    int pid;

    if(built_cmd(cmd, env))
        return;
    else
        pid = fork();
    if(pid == 0)
        child(cmd, env);

}

void    pipes(t_pipeline *cmds, char **env)
{
    int i;
    int fd[2];
    int prev_fd = 0;
    pid_t pid;

    i = 0;
    while (i < cmds->cmd_count)
    {
        if (i < cmds->cmd_count - 1)
            pipe(fd);
        pid = fork();
        if (pid == 0) //* Child process
        {
            if (prev_fd != 0)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < cmds->cmd_count - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            else
                close(fd[1]);
            if(!built_cmd(cmds->commands, env))
                exit(EXIT_SUCCESS);
            //! Execute the command
            exit(0);
        }
        else //* Parent process
        {
            waitpid(pid, NULL, 0);
            if (prev_fd != 0)
                close(prev_fd);
            if (i < cmds->cmd_count - 1)
            {
                close(fd[1]);
                prev_fd = fd[0];
            }
        }
        i++;
    }
}

void    execute(t_pipeline *line, char **env)
{
    if(line->cmd_count == 1)
        single_cmd(line->commands, env);
    else
        pipes(line, env);
}