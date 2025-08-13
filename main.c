#include "minishell.h"

void	executing(char *str, t_env *env_list)
{
	t_token			*tokens;
	t_pipeline		*cmds;
	t_shell_state	shell;

	if (!str || !env_list)
		return ;
	shell.env = env_list;
	shell.last_exit_status = 0;
	if (quote_syntax(str))
	{
		printf("syntax Error\n");
		return ;
	}
	tokens = tokenizer(str);
	if (check_syntax(tokens))
		return ;
	tokens = expand_tokens(tokens, &shell);
	cmds = parse(tokens);
	execute(cmds, &shell);
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
	while (1)
	{
		str = readline("minishell$> ");
		add_to_gc(str);
		if (!str) //? for Ctrl+D
		{
			printf("exit\n");
			break ;
		}
		else
			add_history(str);
		executing(str, state->env);
	}
}
