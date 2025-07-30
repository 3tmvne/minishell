#include "minishell.h"


static int	is_space(char c)
{
	return (c == ' ' || c == '\t');
}

static int	count_words(const char *str)
{
	int i = 0, count = 0;
	char quote = 0;

	while (str[i])
	{
		while (is_space(str[i]))
			i++;
		if (!str[i])
			break;
		count++;
		if (str[i] == '"' || str[i] == '\'')
		{
			quote = str[i++];
			while (str[i] && str[i] != quote)
				i++;
			if (str[i])
				i++;
		}
		else
		{
			while (str[i] && !is_space(str[i]) && str[i] != '"' && str[i] != '\'')
				i++;
		}
	}
	return (count);
}

static char	*word_dup(const char *str, int *i)
{
	int		start = *i;
	char	quote = 0;
	char	*word;

	if (str[*i] == '"' || str[*i] == '\'')
	{
		quote = str[(*i)++];
		while (str[*i] && str[*i] != quote)
			(*i)++;
		if (str[*i] == quote)
			(*i)++;
		word = strndup(&str[start], *i - start);
	}
	else
	{
		while (str[*i] && !is_space(str[*i]) && str[*i] != '"' && str[*i] != '\'')
			(*i)++;
		word = strndup(&str[start], *i - start);
	}
	return (word);
}

static char	*word_dup(const char *str, int *i)
{
    int		start = *i;
    int		len = 0;
    char	quote;

    while (str[*i] && !is_space(str[*i]))
    {
        if (str[*i] == '"' || str[*i] == '\'')
        {
            quote = str[(*i)++];
            while (str[*i] && str[*i] != quote)
                (*i)++;
            if (str[*i] == quote)
                (*i)++;
        }
        else
            (*i)++;
    }
    len = *i - start;
    return strndup(&str[start], len);
}

char	**split_respecting_quotes(const char *str)
{
	char	**result;
	int		i = 0, j = 0;

	int word_count = count_words(str);
	result = malloc(sizeof(char *) * (word_count + 1));
	if (!result)
		return (NULL);
	while (str[i])
	{
		while (is_space(str[i]))
			i++;
		if (!str[i])
			break;
		result[j++] = word_dup(str, &i);
	}
	result[j] = NULL;
	return (result);
}

// int main(void)
// {
//     const char *tests[] = {
//         "echo\"abc def\"\"hell\" ghi",
//         "ls -l 'my file.txt'",
//         "cat 'a b' \"c d\" e",
//         "echo 'a\"b' \"c'd\"",
//         "simple test",
//         "\"all quoted\"",
//         NULL
//     };
//     for (int i = 0; tests[i]; i++)
//     {
//         printf("Input : [%s]\n", tests[i]);
//         char **result = split_respecting_quotes(tests[i]);
//         for (int j = 0; result[j]; j++)
//             printf("  Token %d: [%s]\n", j, result[j]);
//         // Free memory
//         for (int j = 0; result[j]; j++)
//             free(result[j]);
//         free(result);
//         printf("\n");
//     }
//     return 0;
// }
