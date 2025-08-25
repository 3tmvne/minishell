/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:55 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:09:35 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*create_token(const char *value, t_token_type type,
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

t_token	*skip_whitespace(t_token *token)
{
	while (token && token->type == WS)
		token = token->next;
	return (token);
}

int	check_pipe_edges(t_token *current)
{
	if (!current->prev || (current->prev && current->prev->type == WS
			&& !current->prev->prev))
	{
		syntax_error("'|'");
		return (2);
	}
	if (!current->next)
	{
		syntax_error("newline");
		return (2);
	}
	if (current->next && current->next->type == WS && !current->next->next)
	{
		syntax_error("newline");
		return (2);
	}
	return (0);
}

int	check_redirection_edge_cases(t_token *current)
{
	if (!current->next || (current->next && current->next->type == WS
			&& !current->next->next))
	{
		syntax_error("newline");
		return (2);
	}
	if (current->next && current->next->type == REDIR_OUT)
	{
		syntax_error("'>'");
		return (2);
	}
	if (current->next && current->next->type == REDIR_IN)
	{
		syntax_error("'<'");
		return (2);
	}
	return (0);
}
