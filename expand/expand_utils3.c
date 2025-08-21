/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/21 11:20:53 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	merge_concat_into_cur(t_token *cur, t_token *next)
{
	char	*joined;

	if (!cur || !next)
		return ;
	if (cur->quote == NQUOTES && next->quote == DQUOTES
		&& is_special_char(cur->value, '$', 1))
	{
		cur->value = next->value ? ft_strdup(next->value) : ft_strdup("");
		cur->quote = DQUOTES;
		return ;
	}
	if (cur->value && next->value)
		joined = ft_strjoin(cur->value, next->value);
	else if (cur->value)
		joined = ft_strdup(cur->value);
	else if (next->value)
		joined = ft_strdup(next->value);
	else
		joined = ft_strdup("");
	cur->value = joined;
	if (cur->quote != NQUOTES || next->quote != NQUOTES)
		cur->quote = DQUOTES;
}

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

void	merge_assignment_followings(t_token *assign_token)
{
	char	*eq_pos;
	int		name_len;
	char	*full_value, *tmp, *new_assignment;
	t_token	*next, *prev;

	while (assign_token)
	{
		eq_pos = ft_strchr(assign_token->value, '=');
		if (eq_pos)
		{
			name_len = eq_pos - assign_token->value;
			full_value = ft_strdup(eq_pos + 1);
			next = assign_token->next;
			prev = assign_token;
			while (next && next->type == WORD && !ft_strchr(next->value, '='))
			{
				if (!((next->quote == DQUOTES || next->quote == SQUOTES)
						&& (!next->value || next->value[0] == '\0')))
				{
					tmp = ft_strjoin(full_value, next->value);
					full_value = tmp;
				}
				next = next->next;
				prev->next = next;
				if (next)
					next->prev = prev;
			}
			new_assignment = ft_malloc(name_len + 1 + ft_strlen(full_value) + 1);
			if (!new_assignment)
				return ;
			ft_strlcpy(new_assignment, assign_token->value, name_len + 1);
			ft_strlcat(new_assignment, "=", name_len + 2);
			ft_strlcat(new_assignment, full_value, name_len + 2
					+ ft_strlen(full_value));
			assign_token->value = new_assignment;
		}
		assign_token = assign_token->next;
	}
}

void	reconnect_and_split_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*new_head;
	t_token	*next_original;
	t_token	*split_result;
	t_token	*last_split;

	current = tokens;
	new_head = tokens;
	while (current)
	{
		next_original = current->next;
		if (current->type == WORD && current->quote == NQUOTES)
		{
			split_result = split_token_on_whitespace(current);
			if (split_result != current)
			{
				if (current->prev)
				{
					current->prev->next = split_result;
					split_result->prev = current->prev;
				}
				else
				{
					new_head = split_result;
					split_result->prev = NULL;
				}
				last_split = split_result;
				while (last_split->next)
					last_split = last_split->next;
				last_split->next = next_original;
				if (next_original)
					next_original->prev = last_split;
			}
		}
		current = next_original;
	}
	tokens = new_head;
}

static int	is_heredoc_delimiter_token(t_token *current)
{
	t_token	*prev_non_ws;

	prev_non_ws = current->prev;
	while (prev_non_ws)
	{
		if (prev_non_ws->type == WS)
		{
			prev_non_ws = prev_non_ws->prev;
			continue;
		}
		else if (prev_non_ws->type == HEREDOC)
			return (1);
		else if (prev_non_ws->type == WORD)
		{
			prev_non_ws = prev_non_ws->prev;
			continue;
		}
		else
			break;
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

static t_token	*expand_single_token(t_token *current, t_shell_state *shell, t_token **tokens)
{
	t_quote_type	original_quote;
	char			*expanded;

	if (!is_heredoc_delimiter_token(current))
	{
		original_quote = current->quote;
		expanded = expand_token_value(current->value, shell, current->quote);
		if (expanded)
		{
			free(current->value);
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
		if (current->type == WORD && current->value && current->quote != SQUOTES)
			current = expand_single_token(current, shell, &tokens);
		else
			current = current->next;
	}
	return (tokens);
}
