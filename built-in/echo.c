/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:08:43 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/23 17:09:21 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_n_flag(const char *value)
{
	int	i;

	if (!value || value[0] != '-' || value[1] != 'n')
		return (0);
	i = 2;
	while (value[i])
	{
		if (value[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

void	echo(t_cmd *cmd)
{
	int	i;
	int	no_newline;

	if (!cmd || !cmd->args)
		return ;
	no_newline = 0;
	i = 1;
	while (cmd->args[i] && is_valid_n_flag(cmd->args[i]))
	{
		no_newline = 1;
		i++;
	}
	while (cmd->args[i])
	{
		printf("%s", cmd->args[i]);
		if (cmd->args[i + 1])
			printf(" ");
		i++;
	}
	if (!no_newline)
		printf("\n");
}
