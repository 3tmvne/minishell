/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:55:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/23 18:17:54 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_space_token(char *input, int *i, t_token **head,
		t_token **tail)
{
	append(head, tail, get_spaces(input, i));
}

static void	process_quote_token(char *input, int *i, t_token **head,
		t_token **tail)
{
	t_token	*token;

	token = get_quoted(input, i);
	append(head, tail, token);
}

static void	process_word_token(char *input, int *i, t_token **head,
		t_token **tail)
{
	t_token	*token;

	token = get_word(input, i);
	append(head, tail, token);
}

static void	process_special_token(char *input, int *i, t_token **head,
		t_token **tail)
{
	t_token	*token;

	token = get_op(input, i);
	append(head, tail, token);
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
			process_space_token(input, &i, &head, &tail);
		else if (input[i] == '"' || input[i] == '\'')
			process_quote_token(input, &i, &head, &tail);
		else if (is_word(input[i]))
			process_word_token(input, &i, &head, &tail);
		else if (is_special(input[i]))
			process_special_token(input, &i, &head, &tail);
	}
	return (head);
}
