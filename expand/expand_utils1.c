/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/19 03:05:38 by aregragu         ###   ########.fr       */
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

int	contains_whitespace(const char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == ' ' || str[i] == '\t')
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
	while (cur) {
		// Début d'une séquence de tokens non-quotés
		if (cur->type == WORD && cur->quote == NQUOTES) {
			t_token *start = cur;
			t_token *end = cur;
			// Trouver la fin de la séquence non-quotée
			while (end->next && end->next->type == WORD && end->next->quote == NQUOTES)
				end = end->next;
			// Si le suivant est un token quoté, on fait la fusion spéciale
			if (end->next && end->next->type == WORD && end->next->quote != NQUOTES) {
				// Joindre tous les tokens de start à end avec un espace
				size_t total_len = 0;
				t_token *tmp = start;
				int count = 0;
				while (1) {
					total_len += ft_strlen(tmp->value);
					count++;
					if (tmp == end) break;
					tmp = tmp->next;
				}
				total_len += (count - 1); // espaces
				char *joined = ft_malloc(total_len + 1);
				joined[0] = '\0';
				tmp = start;
				for (int i = 0; i < count; i++) {
					ft_strcat(joined, tmp->value);
					if (i != count - 1)
						ft_strcat(joined, " ");
					tmp = tmp->next;
				}
				// Fusionner avec la partie entre quotes
				char *final = ft_strjoin(joined, end->next->value);
				free(joined);
				// Mettre à jour le premier token
				free(start->value);
				start->value = final;
				start->quote = DQUOTES;
				// Supprimer les tokens intermédiaires
				t_token *to_free = start->next;
				while (to_free != end->next) {
					t_token *tmp2 = to_free->next;
					free_token(to_free);
					to_free = tmp2;
				}
				start->next = end->next->next;
				if (start->next)
					start->next->prev = start;
				cur = start->next;
				continue;
			}
			// Sinon, juste merger tous les non-quotés avec espace
			if (start != end) {
				size_t total_len = 0;
				t_token *tmp = start;
				int count = 0;
				while (1) {
					total_len += ft_strlen(tmp->value);
					count++;
					if (tmp == end) break;
					tmp = tmp->next;
				}
				total_len += (count - 1); // espaces
				char *joined = ft_malloc(total_len + 1);
				joined[0] = '\0';
				tmp = start;
				for (int i = 0; i < count; i++) {
					ft_strcat(joined, tmp->value);
					if (i != count - 1)
						ft_strcat(joined, " ");
					tmp = tmp->next;
				}
				free(start->value);
				start->value = joined;
				// Supprimer les tokens intermédiaires
				t_token *to_free = start->next;
				while (to_free != end->next) {
					t_token *tmp2 = to_free->next;
					free_token(to_free);
					to_free = tmp2;
				}
				start->next = end->next;
				if (start->next)
					start->next->prev = start;
				cur = start->next;
				continue;
			}
		}
		cur = cur->next;
	}
	return tokens;
}
