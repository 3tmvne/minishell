#include "minishell.h"

static int	is_space(char c)
{
	return (c == ' ' || c == '\t');
}

static int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '"' || c == '\'');
}
static int	is_word(char c)
{
	return (c != ' ' && c != '\t' && c != '"' && c != '\'' && !is_special(c));
}

int	ft_strcspn(const char *s, const char *reject)
{
	int	i;

	i = 0;
	while (s[i] && !ft_strchr(reject, s[i]))
		i++;
	return (i);
}

char	*char_to_str(char c)
{
	char	*str;

	str = malloc(2);
	if (!str)
		return (NULL);
	str[0] = c;
	str[1] = '\0';
	return (str);
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

static t_token	*create_token(char *value, t_token_type type,
		t_quote_type quote)
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

void	append(t_token **head, t_token **tail, t_token *new_token)
{
	if (!*head)
	{
		*head = new_token;
		*tail = new_token;
	}
	else
	{
		(*tail)->next = new_token;
		new_token->prev = *tail;
		*tail = new_token;
	}
}

t_token	*get_spaces(char *input, int *i)
{
	char	*space;
	t_token	*new_token;

	space = ft_strdup(" ");
	new_token = create_token(space, WS, NQUOTES);
	while (input[*i] && is_space(input[*i]))
	{
		(*i)++;
	}
	return (new_token);
}

t_token	*get_word(char *input, int *i)
{
	t_token	*new_token;
	char	*word;
	int		idx;

	idx = ft_strcspn(&input[*i], " \t\"'|><");
	word = ft_substr(input, *i, idx);
	*i += idx;
	new_token = create_token(word, WORD, NQUOTES);
	return (new_token);
}

t_token	*get_quoted(char *input, int *i)
{
	t_token			*new_token;
	char			*quote;
	t_quote_type	quote_type;
	int				start;
	char			quote_char;

	if (input[*i] == '"')
		quote_type = DQUOTES;
	else if (input[*i] == '\'')
		quote_type = SQUOTES;
	start = *i;
	quote_char = input[start];
	(*i)++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] == quote_char)
		(*i)++;
	quote = ft_substr(input, start + 1, *i - start - 2);
	new_token = create_token(quote, WORD, quote_type);
	return (new_token);
}

t_token	*get_op(char *input, int *i)
{
	t_token	*new_token = NULL;

	if (input[*i] == '|')
	{
		new_token = create_token(char_to_str(input[*i]), PIPE, NQUOTES);
		(*i)++;
	}
	else if (input[*i] == '<' || input[*i] == '>')
	{
		if (input[*i + 1] && input[*i + 1] == input[*i])
		{
			new_token = create_token(ft_substr(input, *i, 2),
					get_token_type(ft_substr(input, *i, 2)), NQUOTES);
			(*i) += 2;
		}
		else
		{
			new_token = create_token(char_to_str(input[*i]),
					get_token_type(char_to_str(input[*i])), NQUOTES);
			(*i)++;
		}
	}
	return (new_token);
}

t_token	*tokenizer(char *input)
{
	t_token	*head;
	t_token	*tail;
	int		i;

	i = 0;
	head = NULL;
	tail = NULL;
	while (input[i])
	{
		if (is_space(input[i]))
			append(&head, &tail, get_spaces(input, &i));
		else if (input[i] == '"' || input[i] == '\'')
			append(&head, &tail, get_quoted(input, &i));
		else if (is_word(input[i]))
			append(&head, &tail, get_word(input, &i));
		else if (is_special(input[i]))
			append(&head, &tail, get_op(input, &i));
	}
	return (head);
}
