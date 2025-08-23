/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 20:28:07 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_whitespace_normalization(const char *str, char *result,
		int *i, int *j)
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

static size_t	calculate_total_length(t_token *start, t_token *end,
		int with_spaces)
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

static void	copy_tokens_to_string(t_token *start, t_token *end, char *joined,
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

t_token	*create_and_add_token(const char *str, int start, int end,
		t_token **first_new, t_token **last_new)
{
	t_token	*new_token;

	new_token = ft_malloc(sizeof(t_token));
	new_token->value = ft_substr(str, start, end - start);
	new_token->type = WORD;
	new_token->quote = NQUOTES;
	new_token->next = NULL;
	/* Gestion de la liaison avec les tokens existants */
	new_token->prev = *last_new;
	if (!*first_new)
		*first_new = new_token;
	else
		(*last_new)->next = new_token;
	*last_new = new_token;
	return (new_token);
}

static void	process_token_splitting(const char *str, t_token **first_new,
		t_token **last_new)
{
	int	start;
	int	i;

	i = 0;
	while (str[i])
	{
		/* Ignorer les espaces au début et entre les mots */
		while (str[i] && (str[i] == ' ' || str[i] == '\t'))
			i++;
		start = i;
		/* Lire jusqu'au prochain espace ou fin de chaîne */
		while (str[i] && !(str[i] == ' ' || str[i] == '\t'))
			i++;
		/* Créer un token si on a trouvé un mot */
		if (i > start)
			create_and_add_token(str, start, i, first_new, last_new);
	}
}

t_token	*split_token_on_whitespace(t_token *token)
{
	t_token		*first_new;
	t_token		*last_new;
	const char	*str;

	first_new = NULL;
	last_new = NULL;
	if (token->quote != NQUOTES || !contains_whitespace(token->value))
		return (token);
	str = token->value;
	process_token_splitting(str, &first_new, &last_new);
	return (first_new ? first_new : token);
}
int	is_special_char(const char *s, char c, int type)
{
	if (type == 1)
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
	return (c == ' ' || c == '\t' || c == '\n');
}

int	contains_whitespace(const char *str)
{
	while (*str)
	{
		if (*str == ' ' || *str == '\t')
			return (1);
		str++;
	}
	return (0);
}
