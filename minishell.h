#ifndef MINISHELL_H
# define MINISHELL_H

# include "./libft/libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
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
	int				save_stdin;
	int				save_stdout;
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
}					t_quote_state;//! 3lash

typedef struct s_parser_state
{
	const char		*input;
	char			*output;
	size_t			in_pos;
	size_t			out_pos;
	size_t			out_capacity;
	t_quote_state quote_state; // Utiliser l'enum au lieu d'int
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
void set_child_signals(void);
t_token				*tokenizer(char *input);
t_pipeline			*parse(t_token *tokens);
int					quote_syntax(char *str);
void				syntax_error(char *token);
int					check_pipe_syntax(t_token *tokens);
int					check_redirection_syntax(t_token *tokens);
int					check_syntax(t_token *tokens);
char				*handle_heredoc_file(char *delimiter, int idx);
int					redirection(t_cmd *cmd);
int					restor_fd(t_cmd *cmd);
int					is_built_cmd(t_cmd *cmd);
void				handle_signals(void);
char	*heredoc(t_cmd *cmd);

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
void				append_output(t_parser_state *ps, const char *str, char c);
int					contains_whitespace(const char *str);
int					is_special_char(const char *s, char c, int type);
t_token				*split_token_on_whitespace(t_token *token);
void				merge_assignment_followings(t_token *assign_token);

/* Functions from expand_utilsX.c */
void				reconnect_and_split_tokens(t_token *tokens);
t_token				*expand_all_word_tokens(t_token *tokens,
						t_shell_state *shell);
char				*expand_token_value(const char *input,
						t_shell_state *shell, t_quote_type quote_type);
char				*normalize_whitespace(const char *str);
char				*join_tokens_with_spaces(t_token *start, t_token *end);
char				*join_tokens_without_spaces(t_token *start, t_token *end);
void				merge_token_operations(t_token *start, t_token *end,
						int type);
t_token				*create_and_add_token(const char *str, int start, int end,
						t_token **first_new, t_token **last_new);
void				handle_quotes(t_parser_state *ps, char c);
void				handle_dollar(t_parser_state *ps);
void				process_character(t_parser_state *ps);
t_parser_state		init_parser_state(const char *input, t_shell_state *shell);
char				*expand_heredoc_line(char *line, t_shell_state *shell);

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