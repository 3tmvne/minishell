#include "minishell.h"

void	print_tokens(t_token *head)
{
    const char *type_str[] = {"WORD", "PIPE", "REDIR_OUT", "REDIR_IN", "APPEND", "HEREDOC"};
    const char *quote_str[] = {"DQUOTES", "SQUOTES", "NQUOTES"};
    int i = 0;
    while (head)
    {
        printf("Node %d:\n", i++);
        printf("  value: '%s'\n", head->value);
        printf("  type: %s\n", type_str[head->type]);
        printf("  quote: %s\n", quote_str[head->quote]);
        head = head->next;
    }
}

void    read_line(void)
{
    char *str;
    t_token *tokens;

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
        if (quote_syntax(str))
            printf("syntax Error\n");
        else
        {
            tokens = lexer(str);
            print_tokens(tokens);
        }
        free(str);
    }
}
