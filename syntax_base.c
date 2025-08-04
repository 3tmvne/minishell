#include "minishell.h"

void	syntax_error(char *token)
{
	fprintf(stderr, "minishell: syntax error near unexpected token '%s'\n", token);
}

void	free_tokens(t_token *tokens)
{
	t_token *tmp;
	
	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
}


//* Fonction simplifiée pour vérifier l'input de base
int	check_input_syntax(char *input)
{
	if (!input || !*input)
		return (0);
	
	//? Vérifier si l'entrée contient seulement des espaces
	int i = 0;
	while (input[i] && isspace(input[i]))
		i++;
	if (!input[i])
		return (0);

	//? Les quotes et autres vérifications sont gérées par le lexer
	return (0);
}
