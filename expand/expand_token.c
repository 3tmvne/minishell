/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:59:16 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/24 15:44:10 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_token_values(t_token *cur, t_token *next)
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

char	*join_tokens(t_token *start, t_token *end, int with_spaces)
{
	size_t	total_len;
	char	*joined;

	total_len = calculate_total_length(start, end, with_spaces);
	joined = ft_malloc(total_len + 1);
	copy_tokens_to_string(start, end, joined, with_spaces);
	joined[total_len] = '\0';
	return (joined);
}

char	*normalize_whitespace(const char *str)
{
	char	*result;
	int		i;
	int		j;

	if (!str || !*str)
		return (NULL);
	result = ft_malloc(ft_strlen(str) + 1);
	i = 0;
	j = 0;
	process_whitespace_normalization(str, result, &i, &j);
	result[j] = '\0';
	return (result);
}

void	append_output(t_parser_state *ps, const char *str, char c)
{
	size_t	len;

	if (str)
	{
		len = ft_strlen(str);
		ensure_capacity(ps, len);
		ft_memcpy(ps->output + ps->out_pos, str, len);
		ps->out_pos += len;
	}
	else if (c)
	{
		ensure_capacity(ps, 1);
		ps->output[ps->out_pos++] = c;
	}
}

void	ensure_capacity(t_parser_state *ps, size_t needed)
{
	size_t	new_cap;
	char	*new_output;

	if (ps->out_pos + needed >= ps->out_capacity)
	{
		new_cap = (ps->out_capacity + needed) * 2;
		new_output = ft_malloc(new_cap);
		if (ps->output)
		{
			ft_memcpy(new_output, ps->output, ps->out_pos);
			add_to_gc(ps->output);
		}
		ps->output = new_output;
		ps->out_capacity = new_cap;
	}
}

