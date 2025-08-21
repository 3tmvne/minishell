/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_base.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:52:04 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 20:58:42 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	syntax_error(char *token)
{
	char	*msg;

	msg = ft_strjoin("minishell: syntax error near unexpected token ", token);
	ft_putstr_fd(msg, 2);
	ft_putchar_fd('\n', 2);
}

int	check_input_syntax(char *input)
{
	int	i;

	if (!input || !*input)
		return (0);
	i = 0;
	while (input[i] && is_space(input[i]))
		i++;
	if (!input[i])
		return (0);
	return (0);
}

int	quote_syntax(char *str)
{
	int		i;
	char	quote_char;

	i = 0;
	if (!str)
		return (1);
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
		{
			quote_char = str[i];
			i++;
			while (str[i] && str[i] != quote_char)
				i++;
			if (str[i] != quote_char)
				return (1);
			i++;
		}
		else
			i++;
	}
	return (0);
}
