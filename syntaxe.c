#include "minishell.h"

void	syntax_error(char *token)
{
	fprintf(stderr, "minishell: syntax error near unexpected token '%s'\n", token);
}

int	is_operator(t_token_type type)
{
	return (type == PIPE);
}

int	is_redirection(t_token_type type)
{
	return (type == REDIR_IN || type == REDIR_OUT || 
			type == APPEND || type == HEREDOC);
}

char	*get_token_string(t_token_type type)
{
	if (type == PIPE)
		return ("|");
	if (type == REDIR_OUT)
		return (">");
	if (type == REDIR_IN)
		return ("<");
	if (type == APPEND)
		return (">>");
	if (type == HEREDOC)
		return ("<<");
	return ("unknown");
}

int	check_quotes(char *input)
{
	int		i;
	int		in_dquote;
	int		in_squote;
	char	last_char;

	i = 0;
	in_dquote = 0;
	in_squote = 0;
	last_char = 0;
	
	while (input[i])
	{
		if (input[i] == '"' && !in_squote && last_char != '\\')
			in_dquote = !in_dquote;
		else if (input[i] == '\'' && !in_dquote && last_char != '\\')
			in_squote = !in_squote;
		else if (input[i] == '\\' && last_char == '\\')
		{
			last_char = 0;
			i++;
			continue;
		}
		last_char = input[i];
		i++;
	}
	
	if (in_dquote)
	{
		syntax_error("\"");
		return (2);
	}
	if (in_squote)
	{
		syntax_error("'");
		return (2);
	}
	
	if (last_char == '\\')
	{
		syntax_error("newline");
		return (2);
	}
	
	return (0);
}

int	check_unsupported_chars(char *input)
{
	int	i;
	int	in_quote;
	char quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	
	while (input[i])
	{
		if (!in_quote && (input[i] == '"' || input[i] == '\''))
		{
			in_quote = 1;
			quote_char = input[i];
		}
		else if (in_quote && input[i] == quote_char)
		{
			in_quote = 0;
			quote_char = 0;
		}
		else if (!in_quote)
		{
			if (input[i] == '`')
			{
				syntax_error("`");
				return (2);
			}
			if (input[i] == '(' || input[i] == ')')
			{
				syntax_error(input[i] == '(' ? "(" : ")");
				return (2);
			}
			if (input[i] == '&' && input[i + 1] != '&')
			{
				syntax_error("&");
				return (2);
			}
			if (input[i] == '&' && input[i + 1] == '&')
			{
				syntax_error("&&");
				return (2);
			}
			if (input[i] == '|' && input[i + 1] == '|')
			{
				syntax_error("||");
				return (2);
			}
			if (input[i] == ';')
			{
				syntax_error(";");
				return (2);
			}
		}
		i++;
	}
	
	return (0);
}


int	check_pipes(t_token *tokens)
{
	t_token	*current;
	int		first_token;

	current = tokens;
	first_token = 1;
	
	while (current)
	{
		if (current->type == PIPE)
		{
			if (first_token)
			{
				syntax_error("|");
				return (2);
			}
			
			if (!current->next || current->next->type == PIPE)
			{
				if (!current->next)
					syntax_error("newline");
				else
					syntax_error("|");
				return (2);
			}
			
			if (current->next && is_operator(current->next->type))
			{
				syntax_error(get_token_string(current->next->type));
				return (2);
			}
		}
		
		if (current->type == WORD)
			first_token = 0;
			
		current = current->next;
	}
	
	return (0);
}

int	check_redirections(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	
	while (current)
	{
		if (is_redirection(current->type))
		{
			// Redirection sans fichier
			if (!current->next || current->next->type != WORD)
			{
				if (!current->next)
					syntax_error("newline");
				else
					syntax_error(get_token_string(current->next->type));
				return (2);
			}
			
			// Redirections consécutives
			if (current->next && current->next->next && is_redirection(current->next->next->type))
			{
				syntax_error(get_token_string(current->next->next->type));
				return (2);
			}
		}
		current = current->next;
	}
	
	return (0);
}

int	check_variables(char *input)
{
	int	i;
	int	in_quote;
	char quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	
	while (input[i])
	{
		if (!in_quote && (input[i] == '"' || input[i] == '\''))
		{
			in_quote = 1;
			quote_char = input[i];
		}
		else if (in_quote && input[i] == quote_char)
		{
			in_quote = 0;
			quote_char = 0;
		}
		else if (!in_quote && input[i] == '$')
		{
			if (input[i + 1] == '{')
			{
				i += 2;
				if (input[i] == '}')
				{
					syntax_error("${}");
					return (2);
				}
				while (input[i] && input[i] != '}')
					i++;
				if (!input[i])
				{
					syntax_error("${");
					return (2);
				}
			}
		}
		i++;
	}
	
	return (0);
}

int	check_syntax(char *input)
{
	t_token	*tokens;
	int		error;

	if (!input || !*input)
		return (0);

	// Vérification des quotes, caractères non supportés, variables
	error = check_quotes(input);
	if (error != 0)
		return (error);

	error = check_unsupported_chars(input);
	if (error != 0)
		return (error);

	error = check_variables(input);
	if (error != 0)
		return (error);

	// Utilise lexer pour transformer l'input en liste chaînée de tokens
	tokens = lexer(input);
	if (!tokens)
		return (0);
	
	error = check_pipes(tokens);
	if (error != 0)
	{
		t_token *tmp;
		while (tokens)
		{
			tmp = tokens;
			tokens = tokens->next;
			free(tmp->value);
			free(tmp);
		}
		return (error);
	}
	
	error = check_redirections(tokens);
	
	t_token *tmp;
	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
	
	return (error);
}

int	main(void)
{
	char	*input;
	int		result;
	
	printf("Testeur de syntaxe Minishell\n");
	printf("============================\n");
	printf("Tapez vos commandes pour tester la syntaxe.\n");
	printf("Tapez 'exit' ou 'quit' pour quitter.\n\n");
	
	while (1)
	{		input = readline("minishell> ");
		
		if (!input)
		{
			printf("\nAu revoir!\n");
			break;
		}
		if (*input)
			add_history(input);
		if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
		{
			free(input);
			printf("Au revoir!\n");
			break;
		}
		if (*input) 
		{
			result = check_syntax(input);
			
			if (result == 0)
				printf("OK: Syntaxe correcte!\n");
			else
				printf("ERREUR: Erreur de syntaxe detectee (code: %d)\n", result);
		}
		free(input);
		printf("\n");
	}
	
	return (0);
}

