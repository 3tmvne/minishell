/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:55:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 20:56:19 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

	token = ft_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = ft_strdup(value);
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
	int				quoted_length;

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
	quoted_length = *i - start - 2;
	if (quoted_length <= 0)
		quote = ft_strdup("");
	else
		quote = ft_substr(input, start + 1, quoted_length);
	new_token = create_token(quote, WORD, quote_type);
	return (new_token);
}

t_token	*get_op(char *input, int *i)
{
	t_token	*new_token;

	new_token = NULL;
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
	t_token	*token;

	i = 0;
	head = NULL;
	tail = NULL;
	while (input[i])
	{
		if (is_space(input[i]))
			append(&head, &tail, get_spaces(input, &i));
		else if (input[i] == '"' || input[i] == '\'')
		{
			token = get_quoted(input, &i);
			append(&head, &tail, token);
		}
		else if (is_word(input[i]))
		{
			token = get_word(input, &i);
			append(&head, &tail, token);
		}
		else if (is_special(input[i]))
		{
			token = get_op(input, &i);
			append(&head, &tail, token);
		}
	}
	return (head);
}
