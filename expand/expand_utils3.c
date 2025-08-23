/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 20:00:49 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*merge_adjacent_words_after_expansion(t_token *tokens)
{
	t_token	*cur;
	t_token	*start;
	t_token	*end;

	cur = tokens;
	while (cur)
	{
		if (cur->type == WORD)
		{
			start = cur;
			end = cur;
			while (end->next && end->next->type == WORD)
				end = end->next;
			if (start != end)
			{
				merge_token_operations(start, end, 0);
				cur = start->next;
				continue ;
			}
		}
		cur = cur->next;
	}
	return (tokens);
}

static int	is_heredoc_delimiter_token(t_token *current)
{
	t_token	*previous;

	previous = current->prev;
	while (previous)
	{
		if (previous->type == WS)
		{
			previous = previous->prev;
			continue ;
		}
		else if (previous->type == HEREDOC)
			return (1);
		else if (previous->type == WORD)
		{
			previous = previous->prev;
			continue ;
		}
		else
			break ;
	}
	return (0);
}

static t_token	*remove_empty_token(t_token *current, t_token **tokens)
{
	t_token	*to_remove;
	t_token	*next_token;

	to_remove = current;
	next_token = current->next;
	if (to_remove->prev)
		to_remove->prev->next = to_remove->next;
	else
		*tokens = to_remove->next;
	if (to_remove->next)
		to_remove->next->prev = to_remove->prev;
	add_to_gc(to_remove->value);
	add_to_gc(to_remove);
	return (next_token);
}

static t_token	*expand_single_token(t_token *current, t_shell_state *shell,
		t_token **tokens)
{
	t_quote_type	original_quote;
	char			*expanded;

	if (!is_heredoc_delimiter_token(current))
	{
		original_quote = current->quote;
		expanded = expand_token_value(current->value, shell, current->quote);
		if (expanded)
		{
			current->value = expanded;
			if (original_quote == DQUOTES)
				current->quote = DQUOTES;
			if (expanded[0] == '\0' && original_quote == NQUOTES)
				return (remove_empty_token(current, tokens));
		}
	}
	return (current->next);
}

t_token	*expand_all_word_tokens(t_token *tokens, t_shell_state *shell)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == WORD && current->value
			&& current->quote != SQUOTES)
			current = expand_single_token(current, shell, &tokens);
		else
			current = current->next;
	}
	return (tokens);
}
