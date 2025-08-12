#include "minishell.h"

void	executing(char *str, t_env *env_list)
{
	t_token *tokens;
	t_pipeline *cmds;
	t_shell_state shell;

	if(!str || !env_list)
		return;
	
	shell.env = env_list;
	shell.last_exit_status = 0;

	if(quote_syntax(str))
	{
		printf("syntax Error\n");
		return;
	}
	tokens = tokenizer(str);
	if(check_syntax(tokens))
		return;
	tokens = expand_tokens(tokens, &shell);
	cmds = parse(tokens);
	execute(cmds, &shell);
}

int	main(int ac, char **av, char **env)
{
	char	*str;
	t_env	*env_list;

	if (ac != 1 || av[1])
	{
		printf("Usage: %s\n", av[0]);
		return (1);
	}
	if (!env || !*env)
	{
		printf("No environment variables found.\n");
		return (1);
	}
	
	// Initialiser la linked list d'environnement
	env_list = array_to_env_list(env);
	if (!env_list)
	{
		printf("Error: Failed to initialize environment\n");
		return (1);
	}
	while (1)
	{
		str = readline("minishell$> ");
		if (!str) //? for Ctrl+D
		{
			printf("exit\n");
			break ;
		}
		else
			add_history(str);
		executing(str, env_list);
	}
	
	// Libérer la linked list d'environnement avant de quitter
	free_env_list(env_list);
}
