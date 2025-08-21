/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/21 09:45:11 by aregragu         ###   ########.fr       */
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
	
	if (tokens->type == WORD && tokens->value && ft_strcmp(tokens->value, "export") == 0)
		return (expand_export_tokens(tokens, shell));
	else
		return (expand_tokens(tokens, shell));
}
