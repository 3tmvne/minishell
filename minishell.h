#ifndef MINISHELL_H
# define MINISHELL_H

# include "./libft/libft.h"
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

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
}					t_pipeline;

typedef struct s_env
{
	char			*name;
	char			*value;
	struct s_env	*next;
}					t_env;

typedef struct s_shell_state
{
	t_env			*env;
	int				last_exit_status;
}					t_shell_state;

typedef enum e_quote_state
{
	STATE_NORMAL,
	STATE_SINGLE,
	STATE_DOUBLE
}					t_quote_state;

typedef struct s_parser_state
{
	const char		*input;
	char			*output;
	size_t			in_pos;
	size_t			out_pos;
	size_t			out_capacity;
	int quote_state; // 0=normal, 1=single, 2=double
	t_shell_state	*shell;
}					t_parser_state;

/* expander state used by the token expansion helpers */
typedef struct s_expander
{
	const char *value;    /* input string being expanded */
	t_quote_type quote;   /* current token quote type */
	t_shell_state *shell; /* shell environment/state */
	char *result;         /* accumulating result string */
	int i;                /* current index in value */
	int in_single_quotes; /* boolean */
	int in_double_quotes; /* boolean */
}					t_expander;

t_token				*tokenizer(char *input);
t_pipeline			*parse(t_token *tokens);
int					quote_syntax(char *str);
void				syntax_error(char *token);
int					check_pipe_syntax(t_token *tokens);
int					check_redirection_syntax(t_token *tokens);
int					check_syntax(t_token *tokens);
int					apply_redirection(t_token *redir);
void				handle_heredoc_file(char *delimiter);

// Environment management functions
t_env				*create_env_node(const char *name, const char *value);
t_env				*find_env_var(t_env *env, const char *name);
char				*get_env_value_list(t_env *env, const char *name);
void				set_env_var(t_env **env, const char *name,
						const char *value);
void				unset_env_var(t_env **env, const char *name);
char				**env_to_array(t_env *env);
t_env				*array_to_env_list(char **env);

// Legacy functions (to be updated)
char				*get_env_value(const char *name, char **env);
void				update_env_value(const char *name, const char *value,
						char **env);
char				*create_env_string(const char *name, const char *value);

void				execute(t_pipeline *line, t_shell_state *shell);
void				pipes(t_pipeline *cmds, t_shell_state *shell);
t_token				*expand_tokens(t_token *tokens, t_shell_state *shell);
t_token				*expand_tokens_selective(t_token *tokens,
						t_shell_state *shell);

/* Expose expansion helper prototypes (previously static in xp.c) */
int					exp_append_char(t_expander *e, char c);
int					exp_append_str(t_expander *e, const char *s);
void				handle_backslash(t_expander *e);
void				handle_single_quote_open(t_expander *e);
void				handle_single_quote_close(t_expander *e);
void				handle_double_quotes(t_expander *e);
void				handle_dollar_outside(t_expander *e);
int					is_all_dollars(const char *s);
int					is_ifs_char(char c);
void				split_words_on_ifs(t_token **head, t_shell_state *shell);
t_token				*merge_adjacent_words_after_expansion(t_token *tokens);

// built cmd
int					built_cmd(t_cmd *cmd, t_shell_state *shell);
void				extern_cmd(t_cmd *cmd, t_shell_state *shell);
void				cd(t_cmd *cmd, t_env **env);
void				env_builtin(t_env *env);
void				echo(t_cmd *cmd);
void				pwd_builtin(void);
int					exit_builtin(t_cmd *cmd, int last_status);
int					export_builtin(t_cmd *cmd, t_env **env);
int					unset_builtin(t_cmd *cmd, t_env **env);
char				**get_filtered_env_list(t_env *env);

/* Helpers used by expand/ implementation */
void				ensure_capacity(t_parser_state *ps, size_t needed);
void				append_char(t_parser_state *ps, char c);
void				append_string(t_parser_state *ps, const char *s);
t_token				*split_token_on_whitespace(t_token *token);
void				merge_assignment_followings(t_token *assign_token);

/* Garbage collector node type used across the project */
typedef struct s_gc
{
	void			*ptr;
	int				flag;
	struct s_gc		*next;
}					t_gc;

// Garbage collector malloc
void				*ft_malloc(size_t size);
void				add_to_gc(void *ptr);
void				free_gc_all(void);
void				free_gc_flag0(void);
void				add_flag_to_gc(t_env *env_node);

#endif