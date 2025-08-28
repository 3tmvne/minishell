/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 14:33:12 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/28 17:41:02 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void	reconnect_split_tokens(t_token *current, t_token *split_result,
// 		t_token *next_original, t_token **new_head)
// {
// 	t_token	*last_split;

// 	if (current->prev)
// 	{
// 		current->prev->next = split_result;
// 		split_result->prev = current->prev;
// 	}
// 	else
// 	{
// 		*new_head = split_result;
// 		split_result->prev = NULL;
// 	}
// 	last_split = split_result;
// 	while (last_split->next)
// 		last_split = last_split->next;
// 	last_split->next = next_original;
// 	if (next_original)
// 		next_original->prev = last_split;
// }

char	*collapse_whitespace(const char *str)
{
	char	*result;
	int		in_space;
	int		i;
	int		j;

	if (!str)
		return (ft_strdup(""));
	i = contains_whitespace(str);
	j = 0;
	in_space = 0;
	result = ft_malloc(ft_strlen(str) + 1);
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
			in_space = 1;
		else
		{
			if (in_space && j > 0)
				result[j++] = ' ';
			result[j++] = str[i];
			in_space = 0;
		}
		i++;
	}
	return (result[j] = '\0', result);
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

// t_token	*split_token_on_whitespace(t_token *token)
// {
// 	t_token		*first_new;
// 	t_token		*last_new;
// 	const char	*str;

// 	first_new = NULL;
// 	last_new = NULL;
// 	if (token->quote != NQUOTES || !contains_whitespace(token->value))
// 		return (token);
// 	str = token->value;
// 	process_token_splitting(str, &first_new);
// 	if (first_new)
// 		return (first_new);
// 	else
// 		return (token);
// }
