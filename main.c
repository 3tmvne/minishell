#include "minishell.h"

void	executing(char *str, t_shell_state *shell)
{
	t_token			*tokens;
	t_pipeline		*cmds;

	if (!str || !shell)
		return ;
	shell->last_exit_status = 0;
	if (quote_syntax(str))
	{
		printf("syntax Error\n");
		return ;
	}
	tokens = tokenizer(str);
	if (check_syntax(tokens))
		return ;
	// Use selective expansion (handles export assignments specially)
	tokens = expand_tokens_selective(tokens, shell);
	cmds = parse(tokens);
	execute(cmds, shell);
}
//! handle !env
int	main(int ac, char **av, char **env)
{
	char			*str;
	t_shell_state	*state;

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	state->env = array_to_env_list(env);
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
			// free_gc_all();
			break ;
		}
		else
			add_history(str);
		executing(str, state);
	}
}
