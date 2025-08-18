#include "minishell.h"

void	executing(char *str, t_shell_state *shell)
{
	t_token			*tokens;
	t_pipeline		*cmds;

	if (!str || !shell)
		return ;
	// Ne pas remettre l'exit status à 0 - garder celui de la commande précédente
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
	execute(cmds, shell);
}

int	main(int ac, char **av, char **env)
{
	char			*str;
	t_shell_state	*state;

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	state->env = array_to_env_list(env);
	state->last_exit_status = 0; // Initialiser l'exit status à 0
	t_env *cur = state->env;
	while (cur) {
		add_flag_to_gc(cur); // set GC flag to 1 for each env node
		cur = cur->next;
	}
	while (1)
	{
		str = readline("minishell$> ");
		add_to_gc(str);
		if (!str) //? for Ctrl+D
		{
			printf("exit\n");
			break ;
		}
		else if (str[0] != '\0')
			add_history(str);
		executing(str, state);
	}
}
