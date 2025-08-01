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
	int		has_command;

	current = tokens;
	has_command = 0;
	
	while (current)
	{
		if (current->type == PIPE)
		{
			// Pipe au début
			if (!has_command)
			{
				syntax_error("|");
				return (2);
			}
			
			// Pipe à la fin ou suivi d'un autre pipe
			if (!current->next)
			{
				syntax_error("newline");
				return (2);
			}
			
			// Reset has_command après un pipe
			has_command = 0;
		}
		else if (current->type == WORD)
		{
			has_command = 1;
		}
		
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
					syntax_error(current->next->value);
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

	// Vérification des quotes de base
	error = quote_syntax(input);
	if (error != 0)
	{
		syntax_error("unclosed quotes");
		return (2);
	}

	// Vérification des caractères non supportés
	error = check_unsupported_chars(input);
	if (error != 0)
		return (error);

	// Vérification des variables
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
		free_tokens(tokens);
		return (error);
	}
	
	error = check_redirections(tokens);
	
	free_tokens(tokens);
	
	return (error);
}

int	main(void)
{
	char	*input;
	int		result;
	
	printf("Testeur de syntaxe Minishell avec Lexer\n");
	printf("=======================================\n");
	printf("Tapez vos commandes pour tester la syntaxe.\n");
	printf("Tapez 'exit' ou 'quit' pour quitter.\n");
	printf("Tapez 'test' pour lancer des tests automatiques.\n\n");
	
	while (1)
	{
		input = readline("minishell> ");
		
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
		
		if (strcmp(input, "test") == 0)
		{
			run_tests();
			free(input);
			printf("\n");
			continue;
		}
		
		if (*input) 
		{
			result = check_syntax(input);
			
			if (result == 0)
				printf("✅ OK: Syntaxe correcte!\n");
			else
				printf("❌ ERREUR: Erreur de syntaxe detectee (code: %d)\n", result);
		}
		
		free(input);
		printf("\n");
	}
	
	return (0);
}

void	run_tests(void)
{
	char *tests_valid[] = {
		"echo hello",
		"ls -la",
		"cat file.txt",
		"echo hello | grep world",
		"ls > output.txt",
		"cat < input.txt",
		"echo hello >> log.txt",
		"cat << EOF",
		"echo \"hello world\"",
		"echo 'single quotes'",
		"echo hello | grep world | wc -l",
		"ls > file && echo done",
		NULL
	};
	
	char *tests_invalid[] = {
		"|",
		"| echo hello",
		"echo hello |",
		"echo hello | | grep world",
		"echo hello >",
		"echo hello <",
		"echo hello >>",
		"echo hello <<",
		"echo hello > | grep world",
		"echo 'unclosed quote",
		"echo \"unclosed quote",
		"echo hello && world",
		"echo hello || world",
		"echo hello; world",
		"echo hello & world",
		"echo hello (world)",
		"echo hello `world`",
		NULL
	};
	
	printf("=== TESTS AUTOMATIQUES ===\n\n");
	
	printf("Tests valides (doivent passer):\n");
	printf("------------------------------\n");
	
	for (int i = 0; tests_valid[i]; i++)
	{
		int result = check_syntax(tests_valid[i]);
		printf("Test %d: %-30s -> %s\n", i + 1, tests_valid[i], 
			   result == 0 ? "✅ PASS" : "❌ FAIL");
	}
	
	printf("\nTests invalides (doivent échouer):\n");
	printf("----------------------------------\n");
	
	for (int i = 0; tests_invalid[i]; i++)
	{
		int result = check_syntax(tests_invalid[i]);
		printf("Test %d: %-30s -> %s\n", i + 1, tests_invalid[i], 
			   result != 0 ? "✅ PASS" : "❌ FAIL");
	}
	
	printf("\n=== FIN DES TESTS ===\n");
}

