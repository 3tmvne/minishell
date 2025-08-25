/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_merge.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 21:48:54 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	merge_token_operations(t_token *start, t_token *end, int type)
{
	char	*joined;

	if (type == 0)
	{
		joined = join_tokens(start, end, 0);
		if (start->quote == DQUOTES || end->quote == DQUOTES)
			start->quote = DQUOTES;
		else
			start->quote = NQUOTES;
		merge_and_update_links(start, joined, end->next, 0);
	}
	else
		handle_complex_merge(start, end);
}

void	handle_complex_merge(t_token *start, t_token *end)
{
	char	*joined;
	char	*final;
	t_token	*next_token;

	joined = join_tokens(start, end, 1);
	next_token = end->next;
	if (next_token && (next_token->quote == SQUOTES
			|| next_token->quote == DQUOTES))
	{
		merge_and_update_links(start, joined, next_token, 1);
		return ;
	}
	final = create_final_merged_value(joined, next_token);
	start->quote = DQUOTES;
	if (next_token)
		merge_and_update_links(start, final, next_token, 0);
	else
		merge_and_update_links(start, final, NULL, 0);
}

void	merge_and_update_links(t_token *start, char *new_value,
		t_token *new_next, int quoted)
{
	if (quoted)
	{
		start->quote = NQUOTES;
		start->value = ft_strdup(new_value);
	}
	else
	{
		start->value = new_value;
	}
	start->next = new_next;
	if (new_next)
		new_next->prev = start;
}

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

int	contains_whitespace(const char *str)
{
	int	i;

	i = 0;
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	return (i);
}
