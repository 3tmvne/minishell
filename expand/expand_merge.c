/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_merge.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/24 15:36:37 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	merge_token_operations(t_token *start, t_token *end, int type)
{
	if (type == 0)
		handle_simple_merge(start, end);
	else
		handle_complex_merge(start, end);
}

void	handle_simple_merge(t_token *start, t_token *end)
{
	char	*joined;

	joined = join_tokens(start, end, 0); // Sans espaces
	if (start->quote == DQUOTES || end->quote == DQUOTES)
		start->quote = DQUOTES;
	else
		start->quote = NQUOTES;
	merge_and_update_links(start, joined, end->next);
}

void	handle_complex_merge(t_token *start, t_token *end)
{
	char	*joined;
	char	*final;
	t_token	*next_token;

	joined = join_tokens(start, end, 1); // Avec espaces
	next_token = end->next;
	// Si le token suivant est quoté, ne pas le fusionner
	if (next_token && (next_token->quote == SQUOTES
			|| next_token->quote == DQUOTES))
	{
		handle_quoted_next_token(start, joined, next_token);
		return ;
	}
	// Fusionner avec le token suivant
	final = create_final_merged_value(joined, next_token);
	start->quote = DQUOTES;
	merge_and_update_links(start, final, next_token ? next_token->next : NULL);
}

void	merge_and_update_links(t_token *start, char *new_value,
		t_token *new_next)
{
	start->value = new_value;
	start->next = new_next;
	if (new_next)
		new_next->prev = start;
}

void	handle_quoted_next_token(t_token *start, char *joined,
		t_token *next_token)
{
	start->quote = NQUOTES;
	merge_and_update_links(start, ft_strdup(joined), next_token);
}

