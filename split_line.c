#include "minishell.h"



t_token *split_line_and_linked(char *str)
{
    char **line;
    int i = 0;
    int count = 0;
    line = ft_split(str, " ");
    
    while(line[count])
        count++;
    while(line[i])
    {
        if(ft_strchr(line[i], '|'))
        {

        }
    }

}