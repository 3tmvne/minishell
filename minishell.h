#ifndef MINISHELL_H
# define MINISHELL_H

# include <signal.h>
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
	HEREDOC,
	WS
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

typedef struct s_cmd
{
	char			**args;
	t_token			*redirections;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_pipeline
{
	t_cmd			*commands;
	int				cmd_count;
} t_pipeline;

t_token				*tokenizer(char *input);
char				**ft_split(char const *s, char c);
int					quote_syntax(char *str);
int					handle_heredoc_file(char *delimiter);
void				syntax_error(char *token);
void				free_tokens(t_token *tokens);
int					check_pipe_syntax(t_token *tokens);
int					check_redirection_syntax(t_token *tokens);
int					check_syntax(t_token *tokens);
void				echo(t_token *tokens);
void				cd(t_token *tokens, char **env);
char				*get_env_value(const char *name, char **env);
void				update_env_value(const char *name, const char *value, char **env);
char				*create_env_string(const char *name, const char *value);
void				env_builtin(char **env);

#endif