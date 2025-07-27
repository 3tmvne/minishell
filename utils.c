#include "minishell.h"

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

int 	ft_strchr(const char *s, char c)
{
	while (*s)
	{
		if (*s == c)
			return (0);
		s++;
	}
	return (1);
}

static int	count_word(const char *s, char c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static char	**ft_free(char **s, int i)
{
	while (i > 0)
		free(s[i--]);
	return (NULL);
}

static int	ft_strlenc(const char *s, char c)
{
	int	i;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	int		i;
	int		j;

	i = 0;
	split = (char **)malloc((count_word(s, c) + 1) * sizeof(char *));
	if (!split || !s)
		return (NULL);
	while (*s)
	{
		if (*s != c)
		{
			j = 0;
			split[i] = (char *)malloc((ft_strlenc(s, c) + 1) * sizeof(char));
			if (!split[i])
				return (ft_free(split, i));
			while (*s && *s != c)
				split[i][j++] = *s++;
			split[i++][j] = '\0';
		}
		else
			s++;
	}
	return (split[i] = NULL, split);
}


// int main()
// {
// 	char **str = ft_split("hello he ll oo", ' ');
// 	int i = 0;
// 	while(str[i])
// 		i++;
		
// 	printf("%d", i);
// }
