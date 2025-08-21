/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/21 21:23:08 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*normalize_whitespace(const char *str)
{
	char	*result;

	int i, j, space_needed;
	if (!str || !*str)
		return (NULL);
	result = ft_malloc(ft_strlen(str) + 1);
	i = 0;
	j = 0;
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	space_needed = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
		{
			if (j > 0)
				space_needed = 1;
		}
		else
		{
			if (space_needed)
			{
				result[j++] = ' ';
				space_needed = 0;
			}
			result[j++] = str[i];
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

static char	*join_tokens_internal(t_token *start, t_token *end, int with_spaces)
{
	size_t	total_len;
	t_token	*tmp;
	char	*joined;
	int		pos;

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
	joined = ft_malloc(total_len + 1);
	pos = 0;
	tmp = start;
	while (tmp)
	{
		ft_strlcpy(joined + pos, tmp->value, total_len + 1 - pos);
		pos += ft_strlen(tmp->value);
		if (tmp == end)
			break ;
		if (with_spaces)
			joined[pos++] = ' ';
		tmp = tmp->next;
	}
	joined[total_len] = '\0';
	return (joined);
}

char	*join_tokens(t_token *start, t_token *end, int with_spaces)
{
	return (join_tokens_internal(start, end, with_spaces));
}

static void	handle_simple_join(t_token *start, t_token *end)
{
	char			*joined;
	t_quote_type	final_quote;

	joined = join_tokens(start, end, 0);
	start->value = joined;
	final_quote = NQUOTES;
	if (start->quote == DQUOTES || end->quote == DQUOTES)
		final_quote = DQUOTES;
	start->quote = final_quote;
	start->next = end->next;
	if (end->next)
		end->next->prev = start;
}

static void	handle_quoted_follow(t_token *start, t_token *end, char *joined)
{
	/* Ne pas fusionner - restituer la valeur originale */
	start->value = ft_strdup(joined);
	start->next = end->next;
	start->quote = NQUOTES;
	/* Reajuster les pointeurs prev */
	if (end != start && end->next)
		end->next->prev = start;
}

static void	handle_complex_join(t_token *start, t_token *end)
{
	t_token	*next_next;

	char *joined, *normalized, *final;
	joined = join_tokens(start, end, 1);
	if (end->next->quote == SQUOTES || end->next->quote == DQUOTES)
	{
		handle_quoted_follow(start, end, joined);
		return ;
	}
	if (end->next->quote == DQUOTES)
		normalized = joined;
	else
		normalized = normalize_whitespace(joined);
	if (normalized && end->next->value)
		final = ft_strjoin(normalized, end->next->value);
	else if (normalized)
		final = ft_strdup(normalized);
	else if (end->next->value)
		final = ft_strjoin(joined, end->next->value);
	else
		final = ft_strdup(joined);
	start->value = final;
	start->quote = DQUOTES;
	next_next = NULL;
	if (end->next)
		next_next = end->next->next;
	start->next = next_next;
	if (next_next)
		next_next->prev = start;
}

static void	handle_end_join(t_token *start, t_token *end)
{
	char *joined, *final;
	joined = join_tokens(start, end, 1);
	final = ft_strdup(joined);
	start->value = final;
	start->quote = DQUOTES;
	start->next = NULL;
}

void	merge_token_operations(t_token *start, t_token *end, int type)
{
	if (type == 0)
		handle_simple_join(start, end);
	else if (end->next)
		handle_complex_join(start, end);
	else
		handle_end_join(start, end);
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

t_token	*split_token_on_whitespace(t_token *token)
{
	t_token		*first_new;
	const char	*str;

	first_new = NULL, *last_new;
	first_new = NULL, last_new = NULL;
	int start, i;
	if (token->quote != NQUOTES || !contains_whitespace(token->value))
		return (token);
	str = token->value;
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
			create_and_add_token(str, start, i, &first_new, &last_new);
	}
	return (first_new ? first_new : token);
}
