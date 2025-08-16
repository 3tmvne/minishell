/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/16 05:05:19 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                           TOKEN PROCESSING                                */
/* ************************************************************************** */

/* The old aggressive merge_adjacent_words was removed because it is unused
 * and its behavior can unintentionally concatenate tokens across IFS splits.
 * Use merge_adjacent_words_after_expansion for conservative merging after
 * expansion (this function is exported and used in the pipeline).
 */

void	merge_assignment_followings(t_token *assign_token)
{
	char	*eq_pos;
	int		name_len;
	char	*full_value;
	t_token	*next;
	t_token	*prev;
	char	*tmp;
	char	*new_assignment;

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
			new_assignment = ft_malloc(name_len + 1 + ft_strlen(full_value)
					+ 1);
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

/* Legacy compatibility functions to keep your existing code working */

char	*collapse_whitespace(const char *str)
{
	char	*result;
	int		i = 0, j;
	int		in_space;
	int		len;

	if (!str)
		return (ft_strdup(""));
	i = 0, j = 0;
	in_space = 0;
	len = ft_strlen(str);
	// Skip leading whitespace
	while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
		i++;
	result = ft_malloc(len + 1);
	if (!result)
		return (NULL);
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			if (!in_space)
			{
				result[j++] = ' ';
				in_space = 1;
			}
		}
		else
		{
			result[j++] = str[i];
			in_space = 0;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

int	is_all_dollars(const char *s)
{
	if (!s || !*s)
		return (0);
	while (*s)
	{
		if (*s != '$')
			return (0);
		s++;
	}
	return (1);
}

int	is_ifs_char(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}
