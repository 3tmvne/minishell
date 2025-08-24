/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/24 19:19:13 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*expand_tokens(t_token *tokens, t_shell_state *shell)
{
	if (!tokens || !shell)
		return (tokens);
	tokens = expand_all_word_tokens(tokens, shell);
	tokens = merge_adjacent_words_after_expansion(tokens);
	reconnect_and_split_tokens(tokens);
	return (tokens);
}

char	*expand_token_value(const char *input, t_shell_state *shell,
		t_quote_type quote_type)
{
	t_parser_state	ps;

	if (!input)
		return (ft_strdup(""));
	ps = init_parser_state(input, shell);
	/* Si le token était entouré de guillemets doubles,
		ne pas traiter les guillemets internes */
	if (quote_type == DQUOTES)
	{
		ps.quote_state = DQUOTES; // Forcer l'état des guillemets doubles
	}
	while (ps.input[ps.in_pos])
		process_character(&ps);
	ensure_capacity(&ps, 1);
	ps.output[ps.out_pos] = '\0';
	return (ps.output);
}

t_parser_state	init_parser_state(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;

	ps.input = input;
	ps.shell = shell;
	ps.in_pos = 0;
	ps.out_pos = 0;
	ps.quote_state = NQUOTES;
	ps.out_capacity = ft_strlen(input) * 2 + 64;
	ps.output = ft_malloc(ps.out_capacity);
	return (ps);
}
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

