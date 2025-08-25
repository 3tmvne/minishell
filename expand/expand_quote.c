/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_quote.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 14:30:44 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 02:52:18 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_quotes(t_parser_state *ps, char c)
{
	if (c == '\'')
	{
		if (ps->quote_state == SQUOTES)
			ps->quote_state = NQUOTES;
		else if (ps->quote_state != DQUOTES)
			ps->quote_state = SQUOTES;
		ps->in_pos++;
	}
	else if (c == '"')
	{
		if (ps->quote_state == DQUOTES)
			ps->quote_state = NQUOTES;
		else if (ps->quote_state != SQUOTES)
			ps->quote_state = DQUOTES;
		ps->in_pos++;
	}
}

static void	handle_dollar_env_var(t_parser_state *ps)
{
	size_t	start;
	size_t	end;
	char	*var_name;
	char	*var_value;

	start = ps->in_pos + 1;
	end = start;
	while (ps->input[end] && (ft_isalnum(ps->input[end])
			|| ps->input[end] == '_'))
		end++;
	if (end > start)
	{
		var_name = ft_substr(ps->input, start, end - start);
		var_value = get_env_value_list(ps->shell->env, var_name);
		if (var_value)
			append_output(ps, var_value, '\0');
		else if (!var_value)
			append_output(ps, "", '\0');
		ps->in_pos = end;
	}
	else
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
	}
}

static void	handle_dollar_expansion(t_parser_state *ps)
{
	int		exit_status;
	char	*status;

	if (ps->input[ps->in_pos + 1] == '?')
	{
		if (get_shell_state(NULL))
			exit_status = get_shell_state(NULL)->last_exit_status;
		else
			exit_status = ps->shell->last_exit_status;
		status = ft_itoa(exit_status);
		append_output(ps, status, '\0');
		ps->in_pos += 2;
	}
	else if (ft_isalpha(ps->input[ps->in_pos + 1]) || ps->input[ps->in_pos
			+ 1] == '_')
	{
		handle_dollar_env_var(ps);
	}
	else
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
	}
}

void	handle_dollar(t_parser_state *ps)
{
	if (ps->quote_state == SQUOTES || !ps->input[ps->in_pos + 1])
	{
		append_output(ps, NULL, '$');
		ps->in_pos++;
		return ;
	}
	handle_dollar_expansion(ps);
}

void	handle_escape_char(t_parser_state *ps)
{
	if (ps->input[ps->in_pos + 1])
	{
		ps->in_pos++;
		append_output(ps, NULL, ps->input[ps->in_pos]);
		ps->in_pos++;
	}
	else
	{
		append_output(ps, NULL, '\\');
		ps->in_pos++;
	}
}
