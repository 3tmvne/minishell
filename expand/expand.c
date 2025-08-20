/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/20 09:48:36 by aregragu         ###   ########.fr       */
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

/* 
 * PIPELINE COMPLET DE TRANSFORMATION
 * 
 * EXEMPLE: echo "Hello $USER"
 * 
 * 1. TOKENIZER: [WORD:"echo"] [WORD:"Hello $USER"]
 * 
 * 2. expand_tokens_selective():
 *    - Vérifie si première commande = "export" → NON
 *    - Appelle expand_tokens()
 * 
 * 3. expand_tokens() Phase 1 - expand_all_word_tokens():
 *    - "Hello $USER" → expand_token_value() → "Hello john"
 *    → [WORD:"echo"] [WORD:"Hello john"]
 * 
 * 4. expand_tokens() Phase 2 - merge_adjacent_words_after_expansion():
 *    → [WORD:"echo"] [WORD:"Hello john"]
 * 
 * 5. expand_tokens() Phase 3 - reconnect_and_split_tokens():
 *    - "Hello john" → ["Hello", "john"]
 *    → [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
 * 
 * 6. RÉSULTAT FINAL: [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
 */