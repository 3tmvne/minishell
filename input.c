#include "minishell.h"

int quote_syntax(char *str)
{
    int i = 0;
    char quote_char;
    
    if (!str)
        return (1);
    while (str[i])
    {
        if (str[i] == '"' || str[i] == '\'')
        {
            quote_char = str[i];
            i++;
            while (str[i] && str[i] != quote_char)
                i++;
            
            if (str[i] != quote_char)
                return (1);
            i++;
        }
        else
            i++;
    }
    return (0);
}
