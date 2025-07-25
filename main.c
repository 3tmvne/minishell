#include "minishell.h"

int command(char **line, char *str)
{
    if (ft_strcmp(line[1], "echo"))
        echo(str);
    // else if (ft_strcmp(line[1], "cd"))
    //     cd(line);
    // else if (ft_strcmp(line[1], "env"))
    //     env(line);
    // else if (ft_strcmp(line[1], "export"))
    //     export(line);
    // else if (ft_strcmp(line[1], "pwd"))
    //     pwd(line);
    // else if (ft_strcmp(line[1], "unset"))
    //     unset(line);
    else
        return 1;
    return 0;
}

void check_command(char **line)
{
    if (line == NULL)
        return;

    if (ft_strcmp(line[0], "exit") == 0)
    {
        free(line);
        exit(0);
    }
    else if (command(line, line[2]))
    {
        printf("Command not found: %s\n", line[0]);
    }

    free(line);
}

void    read_line(void)
{
    // char **line;
    char *str;

    while(1)
    {
        str = readline("minishell$> ");
        if(!str)
        {
            printf("exit\n");
            break;
        }
        else
            add_history(str);
        free(str);
        // check_command(line);
    }
}

int main(void)
{
    read_line();
    return (0);
}
