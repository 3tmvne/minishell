/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 14:04:19 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*normalize_whitespace(const char *str)
{
	char	*result;
	int		i;
	int		j;
	int		space_needed;

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

char	*join_tokens(t_token *start, t_token *end, int with_spaces)
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

static void	merge_and_update_links(t_token *start, char *new_value, t_token *new_next)
{
	start->value = new_value;
	start->next = new_next;
	if (new_next)
		new_next->prev = start;
}

static void	handle_simple_merge(t_token *start, t_token *end)
{
	char	*joined;

	joined = join_tokens(start, end, 0);  // Sans espaces
	if (start->quote == DQUOTES || end->quote == DQUOTES)
		start->quote = DQUOTES;
	else
		start->quote = NQUOTES;
	merge_and_update_links(start, joined, end->next);
}

static void	handle_complex_merge(t_token *start, t_token *end)
{
	char	*joined;
	char	*final;
	t_token	*next_token;

	joined = join_tokens(start, end, 1);  // Avec espaces
	next_token = end->next;
	
	// Si le token suivant est quoté, ne pas le fusionner
	if (next_token && (next_token->quote == SQUOTES || next_token->quote == DQUOTES))
	{
		start->quote = NQUOTES;
		merge_and_update_links(start, ft_strdup(joined), next_token);
		return ;
	}
	
	// Fusionner avec le token suivant
	if (next_token && next_token->value)
	{
		if (next_token->quote == DQUOTES)
			final = ft_strjoin(joined, next_token->value);  // Pas de normalisation
		else
			final = ft_strjoin(normalize_whitespace(joined), next_token->value);
	}
	else
	{
		final = ft_strdup(joined);
	}
	
	start->quote = DQUOTES;
	merge_and_update_links(start, final, next_token ? next_token->next : NULL);
}

void	merge_token_operations(t_token *start, t_token *end, int type)
{
	if (type == 0)
		handle_simple_merge(start, end);
	else
		handle_complex_merge(start, end);
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
	t_token		*last_new;
	const char	*str;
	int			start;
	int			i;

	first_new = NULL;
	last_new = NULL;
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
	return (first_new ? first_new : token);//!!!!!!!!!!!!
}
