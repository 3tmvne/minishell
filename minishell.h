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

typedef struct s_shell_state
{
	char			**env;
	int				last_exit_status;
}					t_shell_state;

t_token				*tokenizer(char *input);
t_pipeline			*parse(t_token *tokens);
int					quote_syntax(char *str);
int					handle_heredoc_file(char *delimiter);
void				syntax_error(char *token);
void				free_tokens(t_token *tokens);
int					check_pipe_syntax(t_token *tokens);
int					check_redirection_syntax(t_token *tokens);
int					check_syntax(t_token *tokens);
char				*get_env_value(const char *name, char **env);
void				update_env_value(const char *name, const char *value, char **env);
char				*create_env_string(const char *name, const char *value);
void				execute(t_pipeline *line, char **env);
void				pipes(t_pipeline *cmds, char **env);
t_token				*expand_tokens(t_token *tokens, t_shell_state *shell);
t_token				*expand_tokens_selective(t_token *tokens, t_shell_state *shell);
char				*expand_token_value(const char *value, t_quote_type quote, t_shell_state *shell);

// Execution helpers
void				child_process(t_token *tokens, t_shell_state *shell);
char				**tokens_to_args(t_token *tokens);
char				*find_command_path(char *cmd, char **env);
void				free_args(char **args);

//built cmd
int					built_cmd(t_cmd *cmd, char **env);
void				extern_cmd(t_cmd *cmd, char **env);
void				cd(t_cmd *cmd, char **env);
void				env_builtin(char **env);
void				echo(t_cmd *cmd);
void				pwd_builtin(void);
int					exit_builtin(t_cmd *cmd, int last_status);
void				export_builtin(t_cmd *cmd, char **env);
void				unset_builtin(t_cmd *cmd, char **env);
char				**get_filtered_env(char **env);

#endif