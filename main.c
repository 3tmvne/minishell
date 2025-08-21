#include "minishell.h"

t_shell_state	*g_shell_state = NULL;

char	*heredoc(t_cmd *cmd)
{
	int		i;
	char	*file;
	t_token	*redirections;

	i = 0;
	file = NULL;
	redirections = cmd->redirections;
	while (redirections)
	{
		if (redirections->type == HEREDOC)
		{
			/* Traiter le heredoc avec le délimiteur et son type de guillemets */
			redirections->value = ft_strdup(handle_heredoc_file(redirections->value,
						i, redirections->quote));
		}
		if (redirections->next == NULL)
			break ;
		redirections = redirections->next;
		i++;
	}
	return (file);
}

void	setup_heredoc(t_cmd *cmd)
{
	t_cmd	*current_cmd;
	t_token	*red;

	current_cmd = cmd;
	while (current_cmd)
	{
		red = current_cmd->redirections;
		if (red)
			heredoc(current_cmd);
		current_cmd = current_cmd->next;
	}
}

void	executing(char *str, t_shell_state *shell)
{
	t_token		*tokens;
	t_pipeline	*cmds;

	// S'assurer que shell est synchronisé avec g_shell_state
	// au cas où g_shell_state aurait été modifié par le gestionnaire de signal
	if (g_shell_state && shell)
	{
		shell->last_exit_status = g_shell_state->last_exit_status;
	}
	if (!str || !shell)
		return ;
	if (quote_syntax(str))
	{
		// Format d'erreur similaire à bash pour les quotes non fermées
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		shell->last_exit_status = 2; // Erreur de syntaxe
		return ;
	}
	tokens = tokenizer(str);
	if (check_syntax(tokens))
	{
		shell->last_exit_status = 2; // Erreur de syntaxe
		return ;
	}
	// Utiliser l'expansion sélective (gère export et cas généraux)
	tokens = expand_tokens_selective(tokens, shell);
	cmds = parse(tokens);
	setup_heredoc(cmds->commands);
	execute(cmds, shell);
}

int	main(int ac, char **av, char **env)
{
	char			*str;
	t_shell_state	*state;
	t_env			*cur;

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	g_shell_state = state;       // Set global shell state
	state->last_exit_status = 0; // Initialiser l'exit status à 0
	state->env = array_to_env_list(env);
	cur = state->env;
	while (cur)
	{
		add_flag_to_gc(cur); // set GC flag to 1 for each env node
		cur = cur->next;
	}
	while (1)
	{
		handle_signals(); // Set up signal handlers
		str = readline("minishell$> ");
		if (!str) //? for Ctrl+D
			exit(state->last_exit_status);
		if (*str) // Empty input
			add_history(str);
		add_to_gc(str);
		executing(str, state);
	}
}
