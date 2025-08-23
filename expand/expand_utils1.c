/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 09:28:44 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Use libft's ft_strcmp implementation; local definition removed. */

void	ensure_capacity(t_parser_state *ps, size_t needed)
{
	size_t	new_cap;
	char	*new_output;

	if (ps->out_pos + needed >= ps->out_capacity)
	{
		new_cap = (ps->out_capacity + needed) * 2;
		new_output = ft_malloc(new_cap);
	   if (ps->output)
	   {
			   ft_memcpy(new_output, ps->output, ps->out_pos);
			   add_to_gc(ps->output);
	   }
		ps->output = new_output;
		ps->out_capacity = new_cap;
	}
}

void	append_output(t_parser_state *ps, const char *str, char c)
{
	size_t	len;

	if (str)
	{
		len = ft_strlen(str);
		ensure_capacity(ps, len);
		ft_memcpy(ps->output + ps->out_pos, str, len);
		ps->out_pos += len;
	}
	else if (c)
	{
		ensure_capacity(ps, 1);
		ps->output[ps->out_pos++] = c;
	}
}

char	*collapse_whitespace(const char *str)
{
	char	*result;
	int		in_space;
	int		i;
	int		j;

	i = 0;
	j = 0;
	in_space = 0;
	if (!str)
		return (ft_strdup(""));
	result = ft_malloc(ft_strlen(str) + 1);
	while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
		i++;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			in_space = 1;
		}
		else
		{
			if (in_space && j > 0)
				result[j++] = ' ';
			result[j++] = str[i];
			in_space = 0;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

int	is_special_char(const char *s, char c, int type)
{
	if (type == 1)
	{
		if (!s || !*s)
			return (0);
		while (*s)
		{
			if (*s != '$')
				return (0);
			s++;
		}
		return (1);
	}
	return (c == ' ' || c == '\t' || c == '\n');
}

int	contains_whitespace(const char *str)
{
	while (*str)
	{
		if (*str == ' ' || *str == '\t')
			return (1);
		str++;
	}
	return (0);
}

char	*expand_heredoc_line(char *line, t_shell_state *shell)
{
	t_parser_state	ps;
	int				i;
	char			*status;
	int				start;
	char			*var_name;
	char			*var_value;

	if (!line)
		return (ft_strdup(""));
	ps = init_parser_state(line, shell);
	ps.quote_state = NQUOTES;
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1])
		{
			if (line[i + 1] == '?')
			{
				status = ft_itoa(shell->last_exit_status);
				append_output(&ps, status, '\0');
				i += 2;
			}
			else if (ft_isalpha(line[i + 1]) || line[i + 1] == '_')
			{
				start = i + 1;
				while (line[i + 1] && (ft_isalnum(line[i + 1]) || line[i
						+ 1] == '_'))
					i++;
				var_name = ft_substr(line, start, i + 1 - start);
				var_value = get_env_value_list(shell->env, var_name);
				if (var_value)
					append_output(&ps, var_value, '\0');
				i++;
			}
			else
			{
				append_output(&ps, NULL, line[i]);
				i++;
			}
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
