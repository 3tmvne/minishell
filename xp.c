/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xp.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 15:00:00 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/15 23:14:53 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/*
 * Collapse multiple whitespace characters into a single space
 * This function is used for variable expansion outside of quotes
 * to match bash behavior
 */
char *collapse_whitespace(const char *str)
{
    char *result;
    int i, j;
    int in_space = 0;
    int len = ft_strlen(str);
    
    // Allocate enough space for the result (worst case: same length as input)
    result = ft_malloc(len + 1);
    if (!result)
        return NULL;
        
    i = 0;
    j = 0;
    
    // Skip leading whitespace
    while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
        i++;
        
    while (str[i])
    {
        // If current character is whitespace
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        {
            // Only add a space if we're not already in a whitespace sequence
            if (!in_space)
            {
                result[j++] = ' ';
                in_space = 1;
            }
        }
        else
        {
            // For non-whitespace characters, just copy
            result[j++] = str[i];
            in_space = 0;
        }
        i++;
    }
    
    // Null terminate the result string
    result[j] = '\0';
    
    return result;
}

static int exp_append_char(t_expander *e, char c)
{
	char *tmp = ft_malloc(ft_strlen(e->result) + 2);
	if (!tmp)
		return (0);
	ft_strlcpy(tmp, e->result, ft_strlen(e->result) + 1);
	tmp[ft_strlen(e->result)] = c;
	tmp[ft_strlen(e->result) + 1] = '\0';
	e->result = tmp;
	return (1);
}

static int exp_append_str(t_expander *e, const char *s)
{
	if (!s || s[0] == '\0')
		return (1);
	char *left;
	if (e->result)
		left = e->result;
	else
		left = "";
	char *joined = ft_strjoin(left, s);
	if (!joined)
		return (0);
	e->result = joined;
	return (1);
}

static void handle_backslash(t_expander *e)
{
	if (e->value[e->i + 1])
	{
		if (e->in_double_quotes)
		{
			if (e->value[e->i + 1] == '$' || e->value[e->i + 1] == '\\' || e->value[e->i + 1] == '"')
			{
				exp_append_char(e, e->value[e->i + 1]);
				e->i += 2;
				return;
			}
			else
			{
				exp_append_char(e, '\\');
				e->i += 1;
				return;
			}
		}
		else
		{
			exp_append_char(e, e->value[e->i + 1]);
			e->i += 2;
			return;
		}
	}
	else
	{
		exp_append_char(e, '\\');
		e->i += 1;
		return;
	}
}

static void handle_single_quote_open(t_expander *e)
{
	e->in_single_quotes = 1;
	e->i++;
}

static void handle_single_quote_close(t_expander *e)
{
	e->in_single_quotes = 0;
	e->i++;
}

static void handle_double_quotes(t_expander *e)
{
	e->in_double_quotes = 1;
	e->i++; // skip opening quote
	while (e->value[e->i] && e->value[e->i] != '"')
	{
		if (e->value[e->i] == '$' && e->value[e->i + 1])
		{
			// handle $? inside double quotes
			if (e->value[e->i + 1] == '?' && (e->value[e->i + 2] == '\0' || e->value[e->i + 2] == '"' || !ft_isalnum(e->value[e->i + 2])))
			{
				char *var_value = ft_itoa(e->shell->last_exit_status);
				exp_append_str(e, var_value);
				e->i += 2;
				continue;
			}
			if (ft_isalpha(e->value[e->i + 1]) || e->value[e->i + 1] == '_')
			{
				int j = e->i + 1;
				while (e->value[j] && (ft_isalnum(e->value[j]) || e->value[j] == '_'))
					j++;
				int var_len = j - e->i - 1;
				char *var_name = ft_substr(e->value, e->i + 1, var_len);
				char *var_value = get_env_value_list(e->shell->env, var_name);
				if (var_value)
					exp_append_str(e, var_value);
				e->i = j;
				continue;
			}
		}
		// normal char in double quotes
		exp_append_char(e, e->value[e->i]);
		e->i++;
	}
	if (e->value[e->i] == '"')
	{
		e->in_double_quotes = 0;
		e->i++;
	}
}

static void handle_dollar_outside(t_expander *e)
{
	if (e->value[e->i + 1] == '?')
	{
		char *var_value = ft_itoa(e->shell->last_exit_status);
		exp_append_str(e, var_value);
		e->i += 2;
		return;
	}
	if (ft_isalpha(e->value[e->i + 1]) || e->value[e->i + 1] == '_')
	{
		int j = e->i + 1;
		while (e->value[j] && (ft_isalnum(e->value[j]) || e->value[j] == '_'))
			j++;
		int var_len = j - e->i - 1;
		char *var_name = ft_substr(e->value, e->i + 1, var_len);
		char *var_value = get_env_value_list(e->shell->env, var_name);
		if (var_value)
		{
			// collapse whitespace when not in double quotes
			if (e->in_double_quotes || e->quote == DQUOTES)
				exp_append_str(e, var_value);
			else
			{
				char *proc = collapse_whitespace(var_value);
				exp_append_str(e, proc);
			}
		}
		e->i = j;
		return;
	}
	// if not alnum after $, just append $ literally
	exp_append_char(e, '$');
	e->i++;
}

char	*expand_token_value(const char *value, t_quote_type quote,
		t_shell_state *shell)
{
	t_expander e;

	// initialize expander
	e.value = value;
	e.quote = quote;
	e.shell = shell;
	e.result = ft_strdup("");
	e.i = 0;
	e.in_single_quotes = 0;
	e.in_double_quotes = (quote == DQUOTES);

	if (quote == SQUOTES)
		return (ft_strdup(value));

	while (e.value[e.i])
	{
		// backslash handling
		if (e.value[e.i] == '\\' && !e.in_single_quotes)
		{
			handle_backslash(&e);
			continue;
		}
		// single quotes
		if (e.value[e.i] == '\'' && !e.in_single_quotes)
		{
			handle_single_quote_open(&e);
			continue;
		}
		if (e.value[e.i] == '\'' && e.in_single_quotes)
		{
			handle_single_quote_close(&e);
			continue;
		}
		if (e.in_single_quotes)
		{
			exp_append_char(&e, e.value[e.i]);
			e.i++;
			continue;
		}
		// double quotes
		if (e.value[e.i] == '"')
		{
			handle_double_quotes(&e);
			continue;
		}
		// dollar outside
		if (e.value[e.i] == '$' && e.value[e.i + 1])
		{
			handle_dollar_outside(&e);
			continue;
		}
		// normal char
		exp_append_char(&e, e.value[e.i]);
		e.i++;
	}
	return (e.result);
}

static int is_all_dollars(const char *s)
{
    if (!s || !*s)
        return 0;
    while (*s)
    {
        if (*s != '$')
            return 0;
        s++;
    }
    return 1;
}

/*
 * Merge adjacent WORD tokens (after expansion) into a single WORD token.
 * This collapses sequences like: WORD("hello") -> WORD("") -> WORD("world")
 * into a single WORD("helloworld") which matches bash behavior for adjacent
 * quoted/unquoted segments.
 */
static t_token *merge_adjacent_words_after_expansion(t_token *tokens)
{
    t_token *cur = tokens;

    while (cur && cur->next)
    {
        if (cur->type == WORD && cur->next->type == WORD)
        {
            t_token *next = cur->next;
            /* General rule: if the left token is composed only of dollar signs
               (e.g. "$"), and it is unquoted, and the right token is a
               double-quoted WORD, then interpret the sequence $"..." by
               dropping the leading dollar operators and keeping the quoted
               content. This generalizes the previous hardcoded check. */
            if (cur->quote == NQUOTES && next->quote == DQUOTES && is_all_dollars(cur->value))
            {
                /* replace next->value (dropping $ signs) */
                char *todup;
                if (next->value)
                    todup = next->value;
                else
                    todup = "";
                cur->value = ft_strdup(todup);
            }
            else
            {
                // Default behavior: concatenate non-empty next->value into cur->value
                if (next->value && next->value[0] != '\0')
                {
                    char *left;
                    if (cur->value)
                        left = cur->value;
                    else
                        left = "";
                    char *joined = ft_strjoin(left, next->value);
                    cur->value = joined;
                }
            }
            // Remove next node from list
            t_token *to_remove = next;
            cur->next = to_remove->next;
            if (to_remove->next)
                to_remove->next->prev = cur;
            // Note: memory for to_remove is GC-managed; do not free
            continue; // stay on same cur to catch multiple consecutive WORDs
        }
        cur = cur->next;
    }
    return tokens;
}

t_token	*expand_tokens(t_token *tokens, t_shell_state *shell)
{
	t_token	*cur;
	char	*expanded_value;

	cur = tokens;
	while (cur)
	{
		// L'expansion ne s'applique qu'aux tokens de type WORD
		if (cur->type == WORD && cur->value)
		{
			expanded_value = expand_token_value(cur->value, cur->quote, shell);
			// cur->value is managed by GC
			cur->value = expanded_value;
		}
		cur = cur->next;
	}

	// Merge adjacent WORD tokens produced by expansion (e.g., empty quoted tokens)
	tokens = merge_adjacent_words_after_expansion(tokens);
	return (tokens);
}

/**
 * Expansion sélective : traitement spécial pour certaines commandes
 */
t_token	*expand_tokens_selective(t_token *tokens, t_shell_state *shell)
{
	t_token	*cur;
	char	*expanded_value;
	char	*cmd_name;

	if (!tokens || tokens->type != WORD)
		return (tokens);
	cmd_name = tokens->value;
	cur = tokens;
	if (ft_strcmp(cmd_name, "export") == 0)
	{
		
		// First, expand all tokens normally
		cur = tokens;
		while (cur)
		{
			if (cur->type == WORD && cur->value)
			{
				expanded_value = expand_token_value(cur->value, cur->quote, shell);
				// cur->value is managed by GC
				cur->value = expanded_value;
			}
			cur = cur->next;
		}
		
		// Then handle special processing for export assignment values
		// Find any assignment tokens
		t_token *assign_token = tokens->next;
		
		while (assign_token)
		{
			char *eq_pos = ft_strchr(assign_token->value, '=');
			if (eq_pos)
			{
				// We found an assignment like zzz=fffff
				// Extract the variable name part before =
				int name_len = eq_pos - assign_token->value;
				char *var_name = ft_substr(assign_token->value, 0, name_len);
				
				// Initialize the value with what's after the = in this token
				// ft_strdup already uses ft_malloc
				char *full_value = ft_strdup(eq_pos + 1);
				t_token *next_token = assign_token->next;
				
				// Process all the following tokens until we hit another assignment or end
				t_token *prev_token = assign_token;
				while (next_token && next_token->type == WORD && !ft_strchr(next_token->value, '='))
				{
					// Skip empty quoted tokens
					if ((next_token->quote == DQUOTES || next_token->quote == SQUOTES) &&
					    (!next_token->value || next_token->value[0] == '\0')) {
						// Skip this token without adding its value
					} else {
						// Append this token's value to the full value
						char *temp = ft_strjoin(full_value, next_token->value);
						// full_value is managed by GC
						full_value = temp;
					}
					
					// Move to the next token
					next_token = next_token->next;
					
					// Remove this token from the linked list
					prev_token->next = next_token;
					if (next_token)
						next_token->prev = prev_token;
					
					// Token is removed - values managed by GC
					// No need to free manually
				}
				
				// Create the new assignment string: name=value
				char *new_assignment = ft_malloc(name_len + 1 + ft_strlen(full_value) + 1);
				ft_strlcpy(new_assignment, var_name, name_len + 1);
				ft_strlcat(new_assignment, "=", name_len + 2);
				ft_strlcat(new_assignment, full_value, name_len + 2 + ft_strlen(full_value));
				
				// Update the token value
				// assign_token->value is managed by GC
				assign_token->value = new_assignment;
				
				// Temporary strings are managed by GC
				
			}
			assign_token = assign_token->next;
		}
	}
	else
	{
		while (cur)
		{
			if (cur->type == WORD && cur->value)
			{
				expanded_value = expand_token_value(cur->value, cur->quote, shell);
				// cur->value is managed by GC
				cur->value = expanded_value;
			}
			cur = cur->next;
		}
	}

	// After selective expansion, merge adjacent WORD tokens as well
	tokens = merge_adjacent_words_after_expansion(tokens);
	return (tokens);
}
