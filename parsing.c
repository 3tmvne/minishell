#include "minishell.h"

int check_syntax_errors(t_token *tokens)
{

}

void parse_line(char *str)
{
    t_token *tokens;

    tokens = split_line(str);
    
    check_syntax_errors(tokens);

}

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
        parse_line(str);
        free(str);
    }
}