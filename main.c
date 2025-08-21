#include "minishell.h"

t_shell_state *g_shell_state = NULL;

char	*heredoc(t_cmd *cmd)
{
	int i = 0;
	char *file = NULL;
	char *heredoc_result = NULL;
	t_token *redirections = cmd->redirections;
	while(redirections)
	{
		if (redirections->type == HEREDOC && redirections->value != NULL)
		{
			int has_quotes = 1; // Supposer qu'il y a des guillemets pour tester
			heredoc_result = handle_heredoc_file(redirections->value, i, has_quotes);
			if (heredoc_result == NULL)
			{
				// Mark this and all remaining heredocs by setting value to NULL
				t_token *remaining = redirections;
				while (remaining)
				{
					if (remaining->type == HEREDOC)
						remaining->value = NULL; // Mark as interrupted
					remaining = remaining->next;
				}
				return (NULL); // Propagate interruption immediately
			}
			redirections->value = ft_strdup(heredoc_result);
		}
		if (redirections->next == NULL)
		break ;
		redirections = redirections->next;
		i++;
	}
	return file;
}

void setup_heredoc(t_cmd *cmd)
{
	t_cmd *current_cmd;
	t_token *red;
	char *result;
	
	current_cmd = cmd;
	while (current_cmd)
	{
		red = current_cmd->redirections;
		if (red)
		{
			result = heredoc(current_cmd);
			if (result == NULL)
				return; // Heredoc was interrupted, stop processing
		}
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
	
	// Synchroniser g_shell_state avec shell après l'exécution
	if (g_shell_state && shell)
	{
		g_shell_state->last_exit_status = shell->last_exit_status;
	}
}

int	main(int ac, char **av, char **env)
{
	char			*str;
	t_shell_state	*state;
	t_env			*cur;

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	g_shell_state = state; // Set global shell state
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
