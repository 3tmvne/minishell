#ifndef MINISHELL_H
# define MINISHELL_H

# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <readline/history.h>
# include <readline/readline.h>
# include "./libft/libft.h"

typedef enum e_token_type
{
	WORD,
	PIPE,
	REDIR_OUT,
	REDIR_IN,
	APPEND,
	HEREDOC
}					t_token_type;

typedef enum e_quote
{
	DQUOTES,
	SQUOTES,
	NQUOTES,
}					t_quote_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote_type	quote;
	struct s_token	*prev;
	struct s_token	*next;
}					t_token;

void                read_line(void);
char				**split_respecting_quotes(const char *str);
char				**ft_split(char const *s, char c);
t_quote_type		get_quote_type(char *str);
char				*add_spaces_around_specials(const char *str);

#endif