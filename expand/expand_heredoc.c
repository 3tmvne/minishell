/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:55:18 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 00:17:42 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_heredoc_exit_status(t_parser_state *ps, t_shell_state *shell,
		int *i)
{
	char	*status;

	status = ft_itoa(shell->last_exit_status);
	append_output(ps, status, '\0');
	*i += 2;
}

static void	handle_heredoc_env_var(t_parser_state *ps, t_shell_state *shell,
		char *line, int *i)
{
	int		start;
	char	*var_name;
	char	*var_value;

	start = *i + 1;
	while (line[*i + 1] && (ft_isalnum(line[*i + 1]) || line[*i + 1] == '_'))
		(*i)++;
	var_name = ft_substr(line, start, *i + 1 - start);
	var_value = get_env_value_list(shell->env, var_name);
	if (var_value)
		append_output(ps, var_value, '\0');
	(*i)++;
}

static void	handle_heredoc_dollar(t_parser_state *ps, t_shell_state *shell,
		char *line, int *i)
{
	if (line[*i + 1] == '?')
	{
		handle_heredoc_exit_status(ps, shell, i);
	}
	else if (ft_isalpha(line[*i + 1]) || line[*i + 1] == '_')
	{
		handle_heredoc_env_var(ps, shell, line, i);
	}
	else
	{
		append_output(ps, NULL, line[*i]);
		(*i)++;
	}
}

char	*expand_heredoc_line(char *line, t_shell_state *shell)
{
	t_parser_state	ps;
	int				i;

	if (!line)
		return (ft_strdup(""));
	ps = init_parser_state(line, shell);
	ps.quote_state = NQUOTES;
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1])
		{
			handle_heredoc_dollar(&ps, shell, line, &i);
		}
		else
		{
			append_output(&ps, NULL, line[i]);
			i++;
		}
	}
	ensure_capacity(&ps, 1);
	ps.output[ps.out_pos] = '\0';
	return (ps.output);
}

void	process_character(t_parser_state *ps)
{
	char	c;

	c = ps->input[ps->in_pos];
	if ((c == '\'' && ps->quote_state != DQUOTES) || (c == '"'
			&& ps->quote_state != SQUOTES))
	{
		handle_quotes(ps, c);
	}
	else if (c == '$')
	{
		handle_dollar(ps);
	}
	else if (c == '\\' && ps->quote_state != SQUOTES)
	{
		handle_escape_char(ps);
	}
	else
	{
		append_output(ps, NULL, c);
		ps->in_pos++;
	}
}
