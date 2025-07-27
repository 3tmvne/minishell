#include "minishell.h"

// void parse_line(char *str)
// {
//     t_token *tokens;

    
    
    

// }

void    read_line(void)
{
    char *str;

    while(1)
    {
        str = readline("minishell$> ");
        if(!str) // for Ctrl+D
        {
            printf("exit\n");
            break;
        }
        else
            add_history(str);
        // parse_line(str);
        free(str);
    }
}