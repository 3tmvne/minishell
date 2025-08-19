/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/19 03:00:34 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
/*                          VARIABLE EXPANSION                               */
/* ************************************************************************** */

static void	expand_exit_status(t_parser_state *ps)
{
	char	*status;

	status = ft_itoa(ps->shell->last_exit_status);
	append_string(ps, status);
	ps->in_pos += 2; // Skip "$?"
}

static void	expand_variable(t_parser_state *ps)
{
	size_t	end;
	char	*var_name;
	char	*var_value;
	size_t start = ps->in_pos + 1; // Skip '$'

	end = start;
	// Find variable name end
	while (ps->input[end] && (ft_isalnum(ps->input[end])
			|| ps->input[end] == '_'))
		end++;
	if (end > start)
	{
		var_name = ft_substr(ps->input, start, end - start);
		var_value = get_env_value_list(ps->shell->env, var_name);
		if (var_value)
			append_string(ps, var_value);
		ps->in_pos = end;
	}
	else
	{
		// Not a valid variable, keep the '$'
		append_char(ps, '$');
		ps->in_pos++;
	}
}

static void	handle_dollar(t_parser_state *ps)
{
	// No expansion in single quotes
	if (ps->quote_state == STATE_SINGLE)
	{
		append_char(ps, '$');
		ps->in_pos++;
		return ;
	}
	// Check what follows $
	if (!ps->input[ps->in_pos + 1])
	{
		append_char(ps, '$');
		ps->in_pos++;
	}
	else if (ps->input[ps->in_pos + 1] == '?')
	{
		expand_exit_status(ps);
	}
	else if (ft_isalpha(ps->input[ps->in_pos + 1]) || ps->input[ps->in_pos
		+ 1] == '_')
	{
		expand_variable(ps);
	}
	else
	{
		append_char(ps, '$');
		ps->in_pos++;
	}
}

/* ************************************************************************** */
/*                            MAIN PARSER                                    */
/* ************************************************************************** */

static void	handle_single_quote(t_parser_state *ps)
{
	if (ps->quote_state == STATE_SINGLE)
		ps->quote_state = STATE_NORMAL;
	else
		ps->quote_state = STATE_SINGLE;
	ps->in_pos++;
}

static void	handle_double_quote(t_parser_state *ps)
{
	if (ps->quote_state == STATE_DOUBLE)
		ps->quote_state = STATE_NORMAL;
	else
		ps->quote_state = STATE_DOUBLE;
	ps->in_pos++;
}

static void	handle_backslash_parser(t_parser_state *ps)
{
	if (ps->input[ps->in_pos + 1])
	{
		ps->in_pos++; // Skip backslash
		append_char(ps, ps->input[ps->in_pos]); // Add escaped char
		ps->in_pos++;
	}
	else
	{
		append_char(ps, '\\'); // Trailing backslash
		ps->in_pos++;
	}
}

static void	process_character(t_parser_state *ps, char c)
{
	// Handle quote state changes
	if (c == '\'' && ps->quote_state != STATE_DOUBLE)
	{
		handle_single_quote(ps);
		return ;
	}
	if (c == '"' && ps->quote_state != STATE_SINGLE)
	{
		handle_double_quote(ps);
		return ;
	}
	// Handle variable expansion
	if (c == '$')
	{
		handle_dollar(ps);
		return ;
	}
	// Handle backslash escaping
	if (c == '\\' && ps->quote_state != STATE_SINGLE)
	{
		handle_backslash_parser(ps);
		return ;
	}
	// Regular character
	append_char(ps, c);
	ps->in_pos++;
}

static t_parser_state	init_parser_state(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;

	// Initialisation manuelle de tous les champs à zéro
	ps.input = input;
	ps.shell = shell;
	ps.in_pos = 0;
	ps.out_pos = 0;
	ps.quote_state = STATE_NORMAL;
	ps.out_capacity = ft_strlen(input) * 2 + 64;
	ps.output = ft_malloc(ps.out_capacity);
	return (ps);
}

static char	*expand_token_value(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;
	char			c;

	if (!input)
		return (ft_strdup(""));
	ps = init_parser_state(input, shell);
	while (ps.input[ps.in_pos])
	{
		c = ps.input[ps.in_pos];
		process_character(&ps, c);
	}
	// Null terminate
	ensure_capacity(&ps, 1);
	ps.output[ps.out_pos] = '\0';
	return (ps.output);
}

/* ************************************************************************** */
/*                              PUBLIC API                                   */
/* ************************************************************************** */

static void	expand_all_word_tokens(t_token *tokens, t_shell_state *shell)
{
	// INPUT: tokens = [WORD:"echo"] [WORD:"Hello $USER"]
	
	t_token	*current;

	current = tokens;
	while (current)
	{
		// Pour chaque token WORD, expand les variables
		if (current->type == WORD && current->value)
		{
			// EXEMPLE: "Hello $USER" → expansion de $USER
			// AVANT: current->value = "Hello $USER"
			current->value = expand_token_value(current->value, shell);
			// APRÈS: current->value = "Hello john"
		}
		current = current->next;
	}
	// OUTPUT: tokens = [WORD:"echo"] [WORD:"Hello john"]
}

static void	reconnect_split_tokens(t_token *current, t_token *split_result,
		t_token *next_original, t_token **new_head)
{
	t_token	*last_split;

	if (current->prev)
	{
		current->prev->next = split_result;
		split_result->prev = current->prev;
	}
	else
	{
		*new_head = split_result;
		split_result->prev = NULL;
	}
	// Find last split token and connect to next
	last_split = split_result;
	while (last_split->next)
		last_split = last_split->next;
	last_split->next = next_original;
	if (next_original)
		next_original->prev = last_split;
}

static t_token	*split_word_tokens_on_whitespace(t_token *tokens)
{
	// INPUT: tokens = [WORD:"echo"] [WORD:"Hello john"]
	
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
			// Split sur les espaces: "Hello john" → ["Hello", "john"]
			split_result = split_token_on_whitespace(current);
			// If token was split, reconnect the list
			if (split_result != current)
			{
				reconnect_split_tokens(current, split_result,
					next_original, &new_head);
			}
		}
		current = next_original;
	}
	// OUTPUT: tokens = [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
	return (new_head);
}

t_token	*expand_tokens(t_token *tokens, t_shell_state *shell)
{
	// INPUT: tokens = [WORD:"echo"] [WORD:"Hello $USER"]
	
	if (!tokens || !shell)
		return (tokens);
		
	// Phase 1: Expansion des variables dans chaque token
	expand_all_word_tokens(tokens, shell);
	// APRÈS Phase 1: tokens = [WORD:"echo"] [WORD:"Hello john"]
	
	// Phase 2: Fusion des mots adjacents (si nécessaire)
	tokens = merge_adjacent_words_after_expansion(tokens);
	// APRÈS Phase 2: tokens = [WORD:"echo"] [WORD:"Hello john"] (pas de changement)
	
	// Phase 3: Word spli4sa", "os"tting sur les espaces
	tokens = split_word_tokens_on_whitespace(tokens);
	// APRÈS Phase 3: tokens = [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
	
	return (tokens);
	// OUTPUT FINAL: [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
}

/* ************************************************************************** */
/*                    FONCTION PRINCIPALE D'EXPANSION                        */
/* ************************************************************************** */

/*
 * TRAÇAGE COMPLET D'UN INPUT : echo "Hello $USER"
 * Environnement : USER=john
 * 
 * ÉTAPE 1 - Après tokenizer :
 * tokens = [WORD:"echo"] [WORD:"Hello $USER"]
 * 
 * ÉTAPE 2 - expand_tokens_selective() :
 * - Vérifie si première commande == "export" → NON
 * - Appelle expand_tokens()
 * 
 * ÉTAPE 3 - expand_tokens() Phase 1 :
 * - Appelle expand_all_word_tokens()
 * - Pour chaque token WORD, appelle expand_token_value()
 *
 * ÉTAPE 4 - expand_token_value("Hello $USER") :
 * - init_parser_state() : ps.input = "Hello $USER"
 * - Boucle caractère par caractère :
 *   'H' → process_character() → append_char('H')
 *   'e' → process_character() → append_char('e') 
 *   'l' → process_character() → append_char('l')
 *   'l' → process_character() → append_char('l')
 *   'o' → process_character() → append_char('o')
 *   ' ' → process_character() → append_char(' ')
 *   '$' → process_character() → handle_dollar()
 *     → expand_variable() → cherche "USER" → trouve "john"
 *     → append_string("john")
 * - Résultat : "Hello john"
 * 
 * ÉTAPE 5 - Après expand_all_word_tokens() :
 * tokens = [WORD:"echo"] [WORD:"Hello john"]
 * 
 * ÉTAPE 6 - expand_tokens() Phase 2 :
 * - merge_adjacent_words_after_expansion() → pas de fusion nécessaire
 * 
 * ÉTAPE 7 - expand_tokens() Phase 3 :
 * - split_word_tokens_on_whitespace()
 * - "Hello john" contient un espace → split en 2 tokens
 * 
 * ÉTAPE FINALE :
 * tokens = [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
 */

static t_token	*expand_export_tokens(t_token *tokens, t_shell_state *shell)
{
	t_token	*cur;

	// Phase 1: Expand all tokens first
	cur = tokens;
	while (cur)
	{
		if (cur->type == WORD && cur->value)
		{
			cur->value = expand_token_value(cur->value, shell);
		}
		cur = cur->next;
	}
	// Phase 2: Special export handling
	merge_assignment_followings(tokens->next);
	// Phase 3: Merge adjacent words produced by expansion (conservative)
	tokens = merge_adjacent_words_after_expansion(tokens);
	// No word splitting for export
	return (tokens);
}

t_token	*expand_tokens_selective(t_token *tokens, t_shell_state *shell)
{
	// INPUT: tokens = [WORD:"echo"] [WORD:"Hello $USER"]
	
	if (!tokens)
		return (tokens);
	
	// Vérification : est-ce "export" ?
	// tokens->value = "echo" ≠ "export"
	if (tokens->type == WORD && tokens->value && ft_strcmp(tokens->value, "export") == 0)
		return (expand_export_tokens(tokens, shell));
	else
		// 🎯 APPEL expand_tokens() pour commande normale
		return (expand_tokens(tokens, shell));
		// OUTPUT: [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
}

/*
 * ═══════════════════════════════════════════════════════════════════════════
 *                        PIPELINE COMPLET DE TRANSFORMATION
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * EXEMPLE DÉTAILLÉ: echo "Hello $USER"
 * 
 * 1️⃣ TOKENIZER (entrée depuis main.c):
 *    → [WORD:"echo"] [WORD:"Hello $USER"]
 * 
 * 2️⃣ expand_tokens_selective():
 *    ✓ Vérifie si première commande = "export" → NON
 *    ✓ Appelle expand_tokens()
 * 
 * 3️⃣ expand_tokens() Phase 1 - expand_all_word_tokens():
 *    ✓ Pour chaque WORD token:
 *      - "echo" → pas de variable → "echo"
 *      - "Hello $USER" → expand_token_value() → "Hello john"
 *    → [WORD:"echo"] [WORD:"Hello john"]
 * 
 * 4️⃣ expand_tokens() Phase 2 - merge_adjacent_words_after_expansion():
 *    ✓ Pas de tokens adjacents à fusionner
 *    → [WORD:"echo"] [WORD:"Hello john"]
 * 
 * 5️⃣ expand_tokens() Phase 3 - split_word_tokens_on_whitespace():
 *    ✓ "echo" → pas d'espace → ["echo"]
 *    ✓ "Hello john" → contient espace → ["Hello", "john"]
 *    → [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
 * 
 * 6️⃣ RÉSULTAT FINAL (retour vers main.c):
 *    → [WORD:"echo"] [WORD:"Hello"] [WORD:"john"]
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 */
