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

int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

char	*add_spaces_around_specials(const char *str)
{
	int		i = 0, j = 0, in_quotes = 0;
	char	quote_char = 0;
	int		len = strlen(str);
	char	*res = malloc(len * 3 + 1);
	if (!res)
		return (NULL);

	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"'))
		{
			if (in_quotes && str[i] == quote_char)
				in_quotes = 0;
			else if (!in_quotes)
			{
				in_quotes = 1;
				quote_char = str[i];
			}
			res[j++] = str[i++];
		}
		else if (!in_quotes && is_special(str[i]))
		{
			res[j++] = ' ';
			if ((str[i] == str[i + 1]) && (str[i] == '<' || str[i] == '>'))
				res[j++] = str[i++];
			res[j++] = str[i++];
			res[j++] = ' ';
		}
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return (res);
}

// int main(void)
// {
//     const char *tests[] = {
//         "echo\"abc\"\"abd\">output",
//         "ls|grep txt",
//         "cat <file",
//         "echo 'a|b' >file",
//         "echo \"a > b\"|cat",
//         "echo abc>>file",
//         "cat <<EOF",
//         NULL
//     };
//     for (int i = 0; tests[i]; i++)
//     {
//         char *result = add_spaces_around_specials(tests[i]);
//         printf("Input : [%s]\nOutput: [%s]\n\n", tests[i], result);
//         free(result);
//     }
//     return 0;
// }
