/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/16 05:20:05 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                            UTILITY FUNCTIONS                              */
/* ************************************************************************** */

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void	ensure_capacity(t_parser_state *ps, size_t needed)
{
	size_t	new_cap;

	if (ps->out_pos + needed >= ps->out_capacity)
	{
		new_cap = (ps->out_capacity + needed) * 2;
		ps->output = realloc(ps->output, new_cap);
		ps->out_capacity = new_cap;
	}
}

void	append_char(t_parser_state *ps, char c)
{
	ensure_capacity(ps, 1);
	ps->output[ps->out_pos++] = c;
}

void	append_string(t_parser_state *ps, const char *str)
{
	size_t	len;

	if (!str)
		return ;
	len = ft_strlen(str);
	ensure_capacity(ps, len);
	ft_memcpy(ps->output + ps->out_pos, str, len);
	ps->out_pos += len;
}

/* ************************************************************************** */
/*                            WORD SPLITTING                                 */
/* ************************************************************************** */

int	contains_whitespace(const char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
			return (1);
	}
	return (0);
}

t_token	*split_token_on_whitespace(t_token *token)
{
	t_token		*first_new = NULL, *last_new;
	const char	*str = token->value;
	int			word_start;
	int			i;
	int			word_end;
	t_token		*new_token;

	// Only split unquoted tokens
	if (token->quote != NQUOTES || !contains_whitespace(token->value))
		return (token);
	first_new = NULL, last_new = NULL;
	word_start = 0;
	i = 0;
	// Skip leading whitespace
	while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
		i++;
	word_start = i;
	while (str[i])
	{
		// Found whitespace or end
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i
			+ 1] == '\0')
		{
			if (str[i + 1] == '\0')
				word_end = i + 1;
			else
				word_end = i;
			if (word_end > word_start)
			{
				// Create new token for this word
				new_token = ft_malloc(sizeof(t_token));
				new_token->value = ft_substr(str, word_start, word_end
						- word_start);
				new_token->type = WORD;
				new_token->quote = NQUOTES;
				new_token->prev = last_new;
				new_token->next = NULL;
				if (!first_new)
				{
					first_new = new_token;
				}
				else
				{
					last_new->next = new_token;
				}
				last_new = new_token;
			}
			// Skip whitespace
			while (str[i] && (str[i] == ' ' || str[i] == '\t'
						|| str[i] == '\n'))
				i++;
			word_start = i;
			i--; // Adjust for loop increment
		}
		i++;
	}
	if (first_new)
		return (first_new);
	return (token);
}

/* Conservative merging used after expansion: join adjacent WORD tokens */
void	merge_concat_into_cur(t_token *cur, t_token *next)
{
	char	*todup;
	char	*left;
	char	*joined;

	if (!cur || !next)
		return ;
	if (cur->quote == NQUOTES && next->quote == DQUOTES
		&& is_all_dollars(cur->value))
	{
		if (next->value)
			todup = next->value;
		else
			todup = "";
		cur->value = ft_strdup(todup);
		return ;
	}
	if (next->value && next->value[0] != '\0')
	{
		if (cur->value)
			left = cur->value;
		else
			left = "";
		joined = ft_strjoin(left, next->value);
		cur->value = joined;
	}
}

t_token	*merge_adjacent_words_after_expansion(t_token *tokens)
{
	t_token	*cur;
	t_token	*next;

	cur = tokens;
	while (cur && cur->next)
	{
		if (cur->type == WORD && cur->next->type == WORD)
		{
			next = cur->next;
			merge_concat_into_cur(cur, next);
			cur->next = next->next;
			if (next->next)
				next->next->prev = cur;
			continue ;
		}
		cur = cur->next;
	}
	return (tokens);
}
