#include "minishell.h"

// void	print_tokens(t_token *head)
// {
//     const char *type_str[] = {"WORD", "PIPE", "REDIR_OUT", "REDIR_IN", "APPEND", "HEREDOC", "WS"};
//     const char *quote_str[] = {"DQUOTES", "SQUOTES", "NQUOTES"};
//     int i = 0;
//     while (head)
//     {
//         printf("Node %d:\n", i++);
//         printf("  value: '%s'\n", head->value);
//         printf("  type: %s\n", type_str[head->type]);
//         printf("  quote: %s\n", quote_str[head->quote]);
//         head = head->next;
//     }
// }

int	main(int ac, char **av, char **env)
{
	char	*str;
	t_token *tokens;
	t_pipeline *pipeline;

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
		if (quote_syntax(str))
		    printf("syntax Error\n");
		tokens = tokenizer(str);
		if(check_syntax(tokens))
			return (1);
		pipeline = parse(tokens);
		if (!pipeline)
			return (1);
		execute(pipeline);
	}
}
