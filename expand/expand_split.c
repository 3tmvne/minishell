/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 21:35:25 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_whitespace_normalization(const char *str, char *result, int *i,
		int *j)
{
	int	space_needed;

	while (str[*i] && (str[*i] == ' ' || str[*i] == '\t'))
		(*i)++;
	space_needed = 0;
	while (str[*i])
	{
		if (str[*i] == ' ' || str[*i] == '\t')
		{
			if (*j > 0)
				space_needed = 1;
		}
		else
		{
			if (space_needed)
			{
				result[(*j)++] = ' ';
				space_needed = 0;
			}
			result[(*j)++] = str[*i];
		}
		(*i)++;
	}
}

size_t	calculate_total_length(t_token *start, t_token *end, int with_spaces)
{
	size_t	total_len;
	t_token	*tmp;

	total_len = 0;
	tmp = start;
	while (tmp)
	{
		total_len += ft_strlen(tmp->value);
		if (tmp != end && with_spaces)
			total_len++;
		if (tmp == end)
			break ;
		tmp = tmp->next;
	}
	return (total_len);
}

void	copy_tokens_to_string(t_token *start, t_token *end, char *joined,
		int with_spaces)
{
	t_token	*tmp;
	int		pos;

	pos = 0;
	tmp = start;
	while (tmp)
	{
		ft_strlcpy(joined + pos, tmp->value, ft_strlen(tmp->value) + 1);
		pos += ft_strlen(tmp->value);
		if (tmp == end)
			break ;
		if (with_spaces)
			joined[pos++] = ' ';
		tmp = tmp->next;
	}
}

void	add_token_to_list(t_token *new_token, t_token **first_new)
{
	t_token	*last;

	if (!new_token || !first_new)
		return ;
	if (!*first_new)
	{
		*first_new = new_token;
		return ;
	}
	last = *first_new;
	while (last->next)
		last = last->next;
	last->next = new_token;
	new_token->prev = last;
}

void	process_token_splitting(const char *str, t_token **first_new)
{
	int		i;
	int		start;
	t_token	*new_token;

	i = 0;
	while (str[i])
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t'))
			i++;
		start = i;
		while (str[i] && !(str[i] == ' ' || str[i] == '\t'))
			i++;
		if (i > start)
		{
			new_token = create_token(str, start, i);
			add_token_to_list(new_token, first_new);
		}
	}
}
