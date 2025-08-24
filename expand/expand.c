/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/24 16:13:18 by aregragu         ###   ########.fr       */
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

t_token	*expand_export_tokens(t_token *tokens, t_shell_state *shell)
{
	tokens = expand_all_word_tokens(tokens, shell);
	merge_assignment_followings(tokens->next);
	tokens = merge_adjacent_words_after_expansion(tokens);
	return (tokens);
}

t_token	*expand_tokens_selective(t_token *tokens, t_shell_state *shell)
{
	if (!tokens)
		return (tokens);
	if (tokens->type == WORD && tokens->value && ft_strcmp(tokens->value,
			"export") == 0)
		return (expand_export_tokens(tokens, shell));
	else
		return (expand_tokens(tokens, shell));
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

// ///////////////////////////////////////////////////////////////




