/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/16 05:20:17 by aregragu         ###   ########.fr       */
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

static char	*expand_token_value(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;
	char			c;

	if (!input)
		return (ft_strdup(""));
	ps = (t_parser_state){0};
	ps.input = input;
	ps.shell = shell;
	ps.out_capacity = ft_strlen(input) * 2 + 64;
	ps.output = ft_malloc(ps.out_capacity);
	while (ps.input[ps.in_pos])
	{
		c = ps.input[ps.in_pos];
		// Handle quote state changes
		if (c == '\'' && ps.quote_state != STATE_DOUBLE)
		{
            if (ps.quote_state == STATE_SINGLE)
                ps.quote_state = STATE_NORMAL;
            else
                ps.quote_state = STATE_SINGLE;
			ps.in_pos++;
			continue ;
		}
		if (c == '"' && ps.quote_state != STATE_SINGLE)
		{
            if (ps.quote_state == STATE_DOUBLE)
                ps.quote_state = STATE_NORMAL;
            else
                ps.quote_state = STATE_DOUBLE;
			ps.in_pos++;
			continue ;
		}
		// Handle variable expansion
		if (c == '$')
		{
			handle_dollar(&ps);
			continue ;
		}
		// Regular character
		append_char(&ps, c);
		ps.in_pos++;
	}
	// Null terminate
	ensure_capacity(&ps, 1);
	ps.output[ps.out_pos] = '\0';
	return (ps.output);
}

/* ************************************************************************** */
/*                              PUBLIC API                                   */
/* ************************************************************************** */

t_token	*expand_tokens(t_token *tokens, t_shell_state *shell)
{
	t_token	*current;
	t_token	*new_head;
	t_token	*next_original;
	t_token	*split_result;
	t_token	*last_split;

	if (!tokens || !shell)
		return (tokens);
	// Phase 1: Expand variables in each token
	current = tokens;
	while (current)
	{
		if (current->type == WORD && current->value)
		{
			current->value = expand_token_value(current->value, shell);
		}
		current = current->next;
	}
	// Phase 2: Merge adjacent word tokens produced by expansion (conservative)
	tokens = merge_adjacent_words_after_expansion(tokens);
	// Phase 3: Split unquoted tokens on whitespace into multiple tokens
	current = tokens;
	new_head = tokens;
	while (current)
	{
		next_original = current->next;
		if (current->type == WORD)
		{
			split_result = split_token_on_whitespace(current);
			// If token was split, reconnect the list
			if (split_result != current)
			{
				if (current->prev)
				{
					current->prev->next = split_result;
					split_result->prev = current->prev;
				}
				else
				{
					new_head = split_result;
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
		}
		current = next_original;
	}
	return (new_head);
}

t_token	*expand_tokens_selective(t_token *tokens, t_shell_state *shell)
{
	char	*cmd_name;
	t_token	*cur;

	if (!tokens || tokens->type != WORD)
		return (tokens);
	cmd_name = tokens->value;
	if (ft_strcmp(cmd_name, "export") == 0)
	{
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
	else
	{
		return (expand_tokens(tokens, shell));
	}
}
