/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:48:15 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 21:48:37 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*create_new_command(void)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	add_argument(t_cmd *cmd, char *arg)
{
	int		count;
	char	**new_args;
	int		i;

	if (!arg)
		return ;
	count = 0;
	if (cmd->args)
	{
		while (cmd->args[count])
			count++;
	}
	new_args = ft_malloc(sizeof(char *) * (count + 2));
	if (cmd->args)
	{
		i = 0;
		while (i < count)
		{
			new_args[i] = cmd->args[i];
			i++;
		}
	}
	new_args[count] = arg;
	new_args[count + 1] = NULL;
	cmd->args = new_args;
}
