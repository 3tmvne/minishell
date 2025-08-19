#include "minishell.h"

t_shell_state *g_shell_state = NULL;

char	*heredoc(t_cmd *cmd)
{
	int i = 0;
	char *file;
	while(cmd->redirections)
	{
		file = handle_heredoc_file(cmd->redirections->value, i);
		if(!cmd->redirections->next)
			break;
		cmd->redirections = cmd->redirections->next;
		i++;
	}
	return file;
}

void	executing(char *str, t_shell_state *shell)
{
	t_token		*tokens;
	t_pipeline	*cmds;

	if (!str || !shell)
		return ;
	// Ne pas remettre l'exit status à 0
		//- garder celui de la commande précédente
	if (quote_syntax(str))
	{
		printf("syntax Error\n");
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
	if (cmds->commands->redirections
		&& cmds->commands->redirections->type == HEREDOC)
		cmds->commands->redirections->value = heredoc(cmds->commands);
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
	g_shell_state = state; // Set global shell state
	state->env = array_to_env_list(env);
	state->last_exit_status = 0; // Initialiser l'exit status à 0
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
		if (g_shell_state->last_exit_status == 130)
		{
			g_shell_state->last_exit_status = 0; // Reset exit status
			continue;
		}
		if (!str) //? for Ctrl+D
		{
			printf("exit\n");
			break ;
		}
		else if (*str) // Empty input
			add_history(str);
		add_to_gc(str);
		executing(str, state);
	}
}
