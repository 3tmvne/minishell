#include "minishell.h"

static t_token	*create_token(char *value, t_token_type type, t_quote_type quote)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = strdup(value);
	token->type = type;
	token->quote = quote;
	token->prev = NULL;
	token->next = NULL;
	return (token);
}

t_token_type	get_token_type(char *word)
{
	if (strcmp(word, "|") == 0)
		return (PIPE);
	else if (strcmp(word, ">") == 0)
		return (REDIR_OUT);
	else if (strcmp(word, ">>") == 0)
		return (APPEND);
	else if (strcmp(word, "<") == 0)
		return (REDIR_IN);
	else if (strcmp(word, "<<") == 0)
		return (HEREDOC);
	else
		return (WORD);
}


t_quote_type	get_quote_type(char *word)
{
	if (word[0] == '"' && word[strlen(word) - 1] == '"')
		return (DQUOTES);
	else if (word[0] == '\'' && word[strlen(word) - 1] == '\'')
		return (SQUOTES);
	else
		return (NQUOTES);
}

t_token	*lexer(char *input)
{
	char	**words;
	t_token	*head = NULL;
	t_token	*current = NULL;
	char	*clean = add_spaces_around_specials(input);
	int		i = 0;

	words = split_respecting_quotes(clean);
	while (words[i])
	{
		t_token *new = create_token(words[i], get_token_type(words[i]), get_quote_type(words[i]));
		if (!head)
			head = new;
		else
		{
			current->next = new;
			new->prev = current;
		}
		current = new;
		i++;
	}
	free(clean);
	return (head);
}

// void	print_tokens(t_token *head)
// {
//     const char *type_str[] = {"WORD", "PIPE", "REDIR_OUT", "REDIR_IN", "APPEND", "HEREDOC"};
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

// int main()
// {
//     char *input = "echo \"abc\"\'hhh\'\"\"output|\'kk\' cat <file";
//     t_token *tokens = lexer(input);
//     print_tokens(tokens);

//     // Free tokens
//     t_token *tmp;
//     while (tokens)
//     {
//         tmp = tokens;
//         tokens = tokens->next;
//         free(tmp->value);
//         free(tmp);
//     }
//     return 0;
// }
