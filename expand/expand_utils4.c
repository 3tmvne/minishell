/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/20 10:54:10 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_shell_state *g_shell_state;

void	handle_quotes(t_parser_state *ps, char c)
{
	if (c == '\'')
	{
		if (ps->quote_state == STATE_SINGLE)
			ps->quote_state = STATE_NORMAL;
		else if (ps->quote_state != STATE_DOUBLE)
			ps->quote_state = STATE_SINGLE;
		ps->in_pos++;
	}
	else if (c == '"')
	{
		if (ps->quote_state == STATE_DOUBLE)
			ps->quote_state = STATE_NORMAL;
		else if (ps->quote_state != STATE_SINGLE)
			ps->quote_state = STATE_DOUBLE;
		ps->in_pos++;
	}
}

static void	handle_dollar_env_var(t_parser_state *ps)
{
	size_t	start, end;
	char	*var_name, *var_value;

	start = ps->in_pos + 1;
	end = start;
	
	/* Extraire le nom de la variable */
	while (ps->input[end] && (ft_isalnum(ps->input[end]) || ps->input[end] == '_'))
		end++;
	
	if (end > start)
	{
		var_name = ft_substr(ps->input, start, end - start);
		var_value = get_env_value_list(ps->shell->env, var_name);
		if (var_value)
			append_output(ps, var_value, '\0');
		ps->in_pos = end;
	}
	else
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
	}
}

void	handle_dollar(t_parser_state *ps)
{
	int		exit_status;
	char	*status;

	/* Cas des guillemets simples */
	if (ps->quote_state == STATE_SINGLE)
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
		return;
	}
	
	/* Cas du $ seul en fin de chaîne */
	if (!ps->input[ps->in_pos + 1])
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
		return;
	}
	
	/* Cas du $? pour le statut de sortie */
	if (ps->input[ps->in_pos + 1] == '?')
	{
		exit_status = g_shell_state ? g_shell_state->last_exit_status : ps->shell->last_exit_status;
		status = ft_itoa(exit_status);
		append_output(ps, status, '\0');
		ps->in_pos += 2;
		return;
	}
	
	/* Cas d'une variable d'environnement */
	if (ft_isalpha(ps->input[ps->in_pos + 1]) || ps->input[ps->in_pos + 1] == '_')
	{
		handle_dollar_env_var(ps);
		return;
	}
	
	/* Cas par défaut: juste un $ littéral */
	append_output(ps, NULL, '$');
	ps->in_pos++;
}

static void	handle_escape_char(t_parser_state *ps)
{
	/* Vérifier s'il y a un caractère après le backslash */
	if (ps->input[ps->in_pos + 1])
	{
		ps->in_pos++;  /* Avancer après le backslash */
		append_output(ps, NULL, ps->input[ps->in_pos]);  /* Ajouter le caractère échappé */
		ps->in_pos++;
	}
	else  /* Backslash en fin de chaîne */
	{
		append_output(ps, NULL, '\\');
		ps->in_pos++;
	}
}

void	process_character(t_parser_state *ps)
{
	char c = ps->input[ps->in_pos];
	
	if ((c == '\'' && ps->quote_state != STATE_DOUBLE) || 
		(c == '"' && ps->quote_state != STATE_SINGLE))
	{
		/* Traiter les guillemets */
		handle_quotes(ps, c);
	}
	else if (c == '$')
	{
		/* Traiter les variables d'environnement */
		handle_dollar(ps);
	}
	else if (c == '\\' && ps->quote_state != STATE_SINGLE)
	{
		/* Traiter les caractères échappés */
		handle_escape_char(ps);
	}
	else
	{
		/* Traiter les caractères normaux */
		append_output(ps, NULL, c);
		ps->in_pos++;
	}
}

t_parser_state	init_parser_state(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;

	ps.input = input;
	ps.shell = shell;
	ps.in_pos = 0;
	ps.out_pos = 0;
	ps.quote_state = STATE_NORMAL;
	ps.out_capacity = ft_strlen(input) * 2 + 64;
	ps.output = ft_malloc(ps.out_capacity);
	return (ps);
}

char	*expand_token_value(const char *input, t_shell_state *shell)
{
	t_parser_state	ps;

	if (!input)
		return (ft_strdup(""));
	ps = init_parser_state(input, shell);
	while (ps.input[ps.in_pos])
		process_character(&ps);
	ensure_capacity(&ps, 1);
	ps.output[ps.out_pos] = '\0';
	return (ps.output);
}
