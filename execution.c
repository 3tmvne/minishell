#include "minishell.h"

child(t_cmd *cmd, char **av)
{
    
}

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

void    pipes(t_pipeline *cmds)
{
    while(cmds->cmd_count--)
    {

    }
}

void    execute(t_pipeline *line, char **env)
{
    if(line->cmd_count == 1)
        single_cmd(line->commands, env);
    else
        pipes(line);
}