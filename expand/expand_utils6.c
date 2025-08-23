/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils6.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:59:16 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 20:27:44 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	reconnect_split_tokens(t_token *current, t_token *split_result,
		t_token *next_original, t_token **new_head)
{
	t_token	*last_split;

	if (current->prev)
	{
		current->prev->next = split_result;
		split_result->prev = current->prev;
	}
	else
	{
		*new_head = split_result;
		split_result->prev = NULL;
	}
	last_split = split_result;
	while (last_split->next)
		last_split = last_split->next;
	last_split->next = next_original;
	if (next_original)
		next_original->prev = last_split;
}

void	reconnect_and_split_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*new_head;
	t_token	*next_original;
	t_token	*split_result;

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
				reconnect_split_tokens(current, split_result, next_original,
					&new_head);
			}
		}
		current = next_original;
	}
	tokens = new_head;
}

static char	*collect_assignment_values(t_token *assign_token,
		t_token **next_ptr)
{
	char	*full_value;
	char	*tmp;
	t_token	*next;
	t_token	*prev;

	full_value = ft_strdup(ft_strchr(assign_token->value, '=') + 1);
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
	*next_ptr = next;
	return (full_value);
}

static void	build_new_assignment(t_token *assign_token, char *full_value)
{
	char	*eq_pos;
	int		name_len;
	char	*new_assignment;

	eq_pos = ft_strchr(assign_token->value, '=');
	name_len = eq_pos - assign_token->value;
	new_assignment = ft_malloc(name_len + 1 + ft_strlen(full_value) + 1);
	ft_strlcpy(new_assignment, assign_token->value, name_len + 1);
	ft_strlcat(new_assignment, "=", name_len + 2);
	ft_strlcat(new_assignment, full_value, name_len + 2
		+ ft_strlen(full_value));
	assign_token->value = new_assignment;
}

void	merge_assignment_followings(t_token *assign_token)
{
	char	*eq_pos;
	char	*full_value;
	t_token	*next;

	while (assign_token)
	{
		eq_pos = ft_strchr(assign_token->value, '=');
		if (eq_pos)
		{
			full_value = collect_assignment_values(assign_token, &next);
			build_new_assignment(assign_token, full_value);
		}
		assign_token = assign_token->next;
	}
}

static char	*join_token_values(t_token *cur, t_token *next)
{
	char	*joined;

	if (cur->value && next->value)
		joined = ft_strjoin(cur->value, next->value);
	else if (cur->value)
		joined = ft_strdup(cur->value);
	else if (next->value)
		joined = ft_strdup(next->value);
	else
		joined = ft_strdup("");
	return (joined);
}

void	merge_concat_into_cur(t_token *cur, t_token *next)
{
	if (!cur || !next)
		return ;
	if (cur->quote == NQUOTES && next->quote == DQUOTES
		&& is_special_char(cur->value, '$', 1))
	{
		cur->value = next->value ? ft_strdup(next->value) : ft_strdup("");
			//!!!!!!!!!!!!!
		cur->quote = DQUOTES;
		return ;
	}
	cur->value = join_token_values(cur, next);
	if (cur->quote != NQUOTES || next->quote != NQUOTES)
		cur->quote = DQUOTES;
}
