/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_assign.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/24 15:40:59 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*create_final_merged_value(char *joined, t_token *next_token)
{
	char	*final;

	if (next_token && next_token->value)
	{
		if (next_token->quote == DQUOTES)
			final = ft_strjoin(joined, next_token->value);
				// Pas de normalisation
		else
			final = ft_strjoin(normalize_whitespace(joined), next_token->value);
	}
	else
	{
		final = ft_strdup(joined);
	}
	return (final);
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

char	*collect_assignment_values(t_token *assign_token,
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

void	build_new_assignment(t_token *assign_token, char *full_value)
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

