/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:02:39 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:40:04 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

typedef struct s_pipe_data
{
	int				prev_fd;
	int				*fd;
	int				i;
	int				cmd_count;
}					t_pipe_data;

typedef struct s_cmd
{
	char			**args;
	int				save_stdin;
	int				save_stdout;
	t_token			*redirections;
	int				heredoc_interrupted;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_pipes_vars
{
	int				fd[2];
	int				prev_fd;
	pid_t			last_pid;
	pid_t			*pids;
	t_cmd			*cmds_list;
	t_pipe_data		data;
}					t_pipes_vars;

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

typedef struct s_parser_state
{
	const char		*input;
	char			*output;
	size_t			in_pos;
	size_t			out_pos;
	size_t			out_capacity;
	t_quote_type	quote_state;
	t_shell_state	*shell;
}					t_parser_state;

typedef struct s_expander
{
	const char		*value;
	t_quote_type	quote;
	t_shell_state	*shell;
	char			*result;
	int				i;
	int				in_single_quotes;
	int				in_double_quotes;
}					t_expander;

typedef struct s_gc
{
	void			*ptr;
	int				flag;
	struct s_gc		*next;
}					t_gc;

void				handler_signal_heredoc(int sig);
void				set_child_signals(void);
t_token				*tokenizer(char *input);
t_token				*create_token(const char *value, t_token_type type,
						t_quote_type quote);
void				append(t_token **head, t_token **tail, t_token *new_token);
t_token				*get_spaces(char *input, int *i);
t_token				*get_word(char *input, int *i);
t_token				*get_quoted(char *input, int *i);
t_token				*get_op(char *input, int *i);
t_token_type		get_token_type(char *word);
t_pipeline			*parse(t_token *tokens);
t_cmd				*create_new_command(void);
void				add_argument(t_cmd *cmd, char *arg);
void				add_redirection(t_cmd *cmd, t_token_type type,
						char *filename, t_quote_type quote_type);
int					quote_syntax(char *str);
void				syntax_error(char *token);
int					check_pipe_syntax(t_token *tokens);
int					check_redirection_syntax(t_token *tokens);
int					check_syntax(t_token *tokens);
char				*handle_heredoc_file(char *delimiter, int idx,
						t_quote_type quote_type);
int					redirection(t_cmd *cmd);
int					restor_fd(t_cmd *cmd);
int					is_built_cmd(t_cmd *cmd);
void				handle_signals(void);
t_env				*create_env_node(const char *name, const char *value);
t_env				*find_env_var(t_env *env, const char *name);
char				*get_env_value_list(t_env *env, const char *name);
void				set_env_var(t_env **env, const char *name,
						const char *value);
void				unset_env_var(t_env **env, const char *name);
char				**env_to_array(t_env *env);
t_env				*array_to_env_list(char **env);
char				*get_env_value(const char *name, char **env);
void				update_env_value(const char *name, const char *value,
						char **env);
char				*create_env_string(const char *name, const char *value);

void				execute(t_pipeline *line, t_shell_state *shell);
void				pipes(t_pipeline *cmds, t_shell_state *shell);
pid_t				*allocate_pids(int cmd_count);
void				setup_child_process(int prev_fd, int *fd, int i,
						int cmd_count);
void				execute_child_command(t_cmd *cmd, t_shell_state *shell);
void				handle_parent_process(int *prev_fd, int *fd, int i,
						int cmd_count);
void				handle_signal_output(int was_interrupted, int last_status);
void				init_pipe_vars(t_pipe_data *data, int *prev_fd);
void				init_pipes_vars(t_pipes_vars *vars, t_pipeline *cmds);
int					wait_single_process(pid_t *pids, int i, pid_t last_pid,
						int *status);
void				process_single_command(t_pipes_vars *vars,
						t_shell_state *shell);
void				wait_for_processes(pid_t *pids, int cmd_count,
						pid_t last_pid, t_shell_state *shell);
pid_t				create_child_process(int *fd, int i, int cmd_count);
t_token				*expand_tokens(t_token *tokens, t_shell_state *shell);
t_token				*merge_adjacent_words_after_expansion(t_token *tokens);

int					built_cmd(t_cmd *cmd, t_shell_state *shell);
int					cd(t_cmd *cmd, t_env **env);
int					echo(t_cmd *cmd);
int					env_builtin(t_cmd *cmd, t_env *env);
int					exit_builtin(t_cmd *cmd, int last_status);
int					export_builtin(t_cmd *cmd, t_env **env);
void				pwd_builtin(void);
int					unset_builtin(t_cmd *cmd, t_env **env);

void				extern_cmd(t_cmd *cmd, t_shell_state *shell);
char				**get_filtered_env_list(t_env *env);
void				ensure_capacity(t_parser_state *ps, size_t needed);
void				append_output(t_parser_state *ps, const char *str, char c);
int					contains_whitespace(const char *str);
int					is_special_char(const char *s, char c, int type);
t_token				*split_token_on_whitespace(t_token *token);
void				reconnect_and_split_tokens(t_token *tokens);
t_token				*expand_all_word_tokens(t_token *tokens,
						t_shell_state *shell);
char				*expand_token_value(const char *input, t_shell_state *shell,
						t_quote_type quote_type);
char				*normalize_whitespace(const char *str);
void				merge_token_operations(t_token *start, t_token *end,
						int type);
void				handle_quotes(t_parser_state *ps, char c);
void				handle_dollar(t_parser_state *ps);
void				process_character(t_parser_state *ps);
t_parser_state		init_parser_state(const char *input, t_shell_state *shell);
char				*expand_heredoc_line(char *line, t_shell_state *shell);
void				*ft_malloc(size_t size);
void				add_to_gc(void *ptr);
void				free_gc_all(void);
int					is_space(char c);
int					is_special(char c);
int					is_word(char c);
int					ft_strcspn(const char *s, const char *reject);
char				*char_to_str(char c);
int					count_env_nodes(t_env *env);
char				**create_sorted_env_for_export(t_env *env);
void				print_env_line(char *env_str);
char				*extract_var_name(const char *var);
char				*clean_value(char *value);
void				add_new_var_to_list(t_env **env, const char *name);
int					count_vars_with_value(t_env *env);
char				*create_filtered_env_string(t_env *current);
char				**get_filtered_env_list(t_env *env);
int					is_valid_varname(const char *name);
t_shell_state		*get_shell_state(t_shell_state *shell_state);
int					heredoc(t_cmd *cmd);
int					setup_heredoc(t_cmd *cmd);
void				print_and_exit_external_error(const char *cmd, int err);
char				*find_command_path(const char *cmd, t_env *env, int *err);
char				*check_absolute_path(const char *cmd, int *err);
char				*join_tokens(t_token *start, t_token *end, int with_spaces);
void				process_whitespace_normalization(const char *str,
						char *result, int *i, int *j);
size_t				calculate_total_length(t_token *start, t_token *end,
						int with_spaces);
void				copy_tokens_to_string(t_token *start, t_token *end,
						char *joined, int with_spaces);
void				process_token_splitting(const char *str,
						t_token **first_new);
char				*join_token_values(t_token *cur, t_token *next);
void				handle_complex_merge(t_token *start, t_token *end);
char				*create_final_merged_value(char *joined,
						t_token *next_token);
void				handle_escape_char(t_parser_state *ps);
void				reconnect_split_tokens(t_token *current,
						t_token *split_result, t_token *next_original,
						t_token **new_head);
void				handle_escape_char(t_parser_state *ps);
t_token				*expand_single_token(t_token *current, t_shell_state *shell,
						t_token **tokens);
int					is_heredoc_delimiter_token(t_token *current);
t_token				*remove_empty_token(t_token *current, t_token **tokens);
int					check_pipe_edges(t_token *current);
int					is_redirection_valid(t_token *redir_token);
int					check_redirection_syntax(t_token *tokens);
int					check_pipe_syntax(t_token *tokens);
int					check_pipe_edges(t_token *current);
int					is_valid_after_pipe(t_token *token);
int					check_syntax(t_token *tokens);
int					is_redirection_valid(t_token *redir_token);
int					check_redirection_syntax(t_token *tokens);
int					check_pipe_syntax(t_token *tokens);
void				syntax_error(char *token);
void				merge_and_update_links(t_token *start, char *new_value,
						t_token *new_next, int quoted);
int					contains_whitespace(const char *str);
t_token				*skip_whitespace(t_token *token);
int					check_redirection_edge_cases(t_token *current);
int					check_redirection_after_pipe(t_token *redir_token);
int					check_redirection_after_pipe_helper(t_token *next_token);
char				*heredoc_filename(int idx);

#endif