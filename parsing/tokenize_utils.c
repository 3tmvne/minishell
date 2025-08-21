/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:56:31 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 20:56:58 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_space(char c)
{
	return (c == ' ' || c == '\t');
}

int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '"' || c == '\'');
}

int	is_word(char c)
{
	return (c != ' ' && c != '\t' && c != '"' && c != '\'' && !is_special(c));
}

int	ft_strcspn(const char *s, const char *reject)
{
	int	i;

	i = 0;
	while (s[i] && !ft_strchr(reject, s[i]))
		i++;
	return (i);
}

char	*char_to_str(char c)
{
	char	*str;

	str = ft_malloc(2);
	if (!str)
		return (NULL);
	str[0] = c;
	str[1] = '\0';
	return (str);
}
