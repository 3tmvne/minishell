/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/19 23:57:19 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/* ************************************************************************** */
/*                            UTILITY FUNCTIONS                              */
/* ************************************************************************** */

/* Use libft's ft_strcmp implementation; local definition removed. */

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

static char	*normalize_whitespace(const char *str)
{
	char	**words;
	char	*normalized;
	size_t	norm_len;
	int		word_count;
	int		i;
	int		first;

	if (!str || ft_strlen(str) == 0)
		return (NULL);
	words = ft_split(str, ' ');
	if (!words)
		return (NULL);
	norm_len = 0;
	word_count = 0;
	i = 0;
	while (words[i])
	{
		if (ft_strlen(words[i]) > 0)
		{
			norm_len += ft_strlen(words[i]);
			word_count++;
		}
		i++;
	}
	if (word_count > 0)
		norm_len += word_count - 1;
	normalized = ft_malloc(norm_len + 1);
	normalized[0] = '\0';
	first = 1;
	i = 0;
	while (words[i])
	{
		if (ft_strlen(words[i]) > 0)
		{
			if (!first)
				ft_strlcat(normalized, " ", norm_len + 1);
			ft_strlcat(normalized, words[i], norm_len + 1);
			first = 0;
		}
		i++;
	}
	i = 0;
	while (words[i])
	{
		free(words[i]);
		i++;
	}
	free(words);
	return (normalized);
}

static char	*join_tokens_with_spaces(t_token *start, t_token *end)
{
	size_t	total_len;
	t_token	*tmp;
	int		count;
	char	*joined;
	int		i;

	total_len = 0;
	tmp = start;
	count = 0;
	while (1)
	{
		total_len += ft_strlen(tmp->value);
		count++;
		if (tmp == end)
			break;
		tmp = tmp->next;
	}
	total_len += (count - 1);
	joined = ft_malloc(total_len + 1);
	joined[0] = '\0';
	tmp = start;
	i = 0;
	while (i < count)
	{
		ft_strlcat(joined, tmp->value, total_len + 1);
		if (i != count - 1)
			ft_strlcat(joined, " ", total_len + 1);
		tmp = tmp->next;
		i++;
	}
	return (joined);
}

static void	remove_intermediate_tokens(t_token *start, t_token *end_next)
{
	t_token	*to_free;
	t_token	*tmp2;

	to_free = start->next;
	while (to_free != end_next)
	{
		tmp2 = to_free->next;
		to_free = tmp2;
	}
	start->next = end_next;
	if (start->next)
		start->next->prev = start;
}

static void	merge_with_quoted_token(t_token *start, t_token *end)
{
	char	*joined;
	char	*normalized;
	char	*to_concat;
	char	*final;

	joined = join_tokens_with_spaces(start, end);
	normalized = normalize_whitespace(joined);
	if (normalized)
		to_concat = normalized;
	else
		to_concat = joined;
	final = ft_strjoin(to_concat, end->next->value);
	free(joined);
	if (normalized)
		free(normalized);
	free(start->value);
	start->value = final;
	start->quote = DQUOTES;
	remove_intermediate_tokens(start, end->next->next);
}

static void	merge_unquoted_tokens(t_token *start, t_token *end)
{
	char	*joined;

	joined = join_tokens_with_spaces(start, end);
	free(start->value);
	start->value = joined;
	remove_intermediate_tokens(start, end->next);
}

int	contains_whitespace(const char *str)
{
	int i = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
			return (1);
		i++;
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
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	word_start = i;
	while (str[i])
	{
		// Found whitespace
		if (str[i] == ' ' || str[i] == '\t')
		{
			word_end = i;
			if (word_end > word_start)
			{
				// Create new token for this word
				new_token = ft_malloc(sizeof(t_token));
				new_token->value = ft_substr(str, word_start, word_end - word_start);
				new_token->type = WORD;
				new_token->quote = NQUOTES;
				new_token->prev = last_new;
				new_token->next = NULL;
				if (!first_new)
					first_new = new_token;
				else
					last_new->next = new_token;
				last_new = new_token;
			}
			// Skip whitespace
			while (str[i] && (str[i] == ' ' || str[i] == '\t'))
				i++;
			word_start = i;
			i--; // Adjust for loop increment
		}
		i++;
	}
	// Add the last word if any
	word_end = i;
	if (word_end > word_start)
	{
		new_token = ft_malloc(sizeof(t_token));
		new_token->value = ft_substr(str, word_start, word_end - word_start);
		new_token->type = WORD;
		new_token->quote = NQUOTES;
		new_token->prev = last_new;
		new_token->next = NULL;
		if (!first_new)
			first_new = new_token;
		else
			last_new->next = new_token;
		last_new = new_token;
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
	if (cur->quote == NQUOTES && next->quote == DQUOTES && is_all_dollars(cur->value))
	{
		if (next->value)
			todup = next->value;
		else
			todup = "";
		cur->value = ft_strdup(todup);
		cur->quote = DQUOTES;
		return ;
	}
	// Toujours concaténer, même si next->value est vide
	if (cur->value)
		left = cur->value;
	else
		left = "";
	if (next->value)
		joined = ft_strjoin(left, next->value);
	else
		joined = ft_strdup(left);
	cur->value = joined;
	// Si l'un des deux tokens est quoté, le résultat doit être considéré comme quoté
	if (cur->quote != NQUOTES || next->quote != NQUOTES)
		cur->quote = DQUOTES;
}

// Fusionne tous les tokens non-quotés consécutifs en un seul séparé par un espace,
// puis concatène ce résultat avec la partie entre quotes si elle existe (bash-like)
t_token *merge_adjacent_words_after_expansion(t_token *tokens)
{
	t_token *cur = tokens;
	
	while (cur) 
	{
		if (cur->type == WORD && cur->quote == NQUOTES) 
		{
			t_token *start = cur;
			t_token *end = cur;
			
			// Trouver la fin de la séquence non-quotée
			while (end->next && end->next->type == WORD && end->next->quote == NQUOTES)
				end = end->next;
				
			// Si le suivant est un token quoté, faire la fusion spéciale
			if (end->next && end->next->type == WORD && end->next->quote != NQUOTES) 
			{
				merge_with_quoted_token(start, end);
				cur = start->next;
				continue;
			}
			
			// Sinon, juste merger tous les non-quotés avec espace
			if (start != end) 
			{
				merge_unquoted_tokens(start, end);
				cur = start->next;
				continue;
			}
		}
		cur = cur->next;
	}
	return tokens;
}
