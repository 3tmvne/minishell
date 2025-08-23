/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:19:06 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/23 17:30:31 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	heredoc(t_cmd *cmd)
{
	int		i;
	t_token	*red;
	char	*heredoc_file;

	i = 0;
	red = cmd->redirections;
	while (red)
	{
		if (red->type == HEREDOC)
		{
			heredoc_file = handle_heredoc_file(red->value, i, red->quote);
			if (!heredoc_file)
				return (1);
			if (get_shell_state(NULL)->last_exit_status == 130)
				return (1);
			red->value = ft_strdup(heredoc_file);
		}
		red = red->next;
		i++;
	}
	return (0);
}

int	setup_heredoc(t_cmd *cmd)
{
	t_token	*red;
	t_cmd	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		red = current_cmd->redirections;
		if (red)
		{
			if (heredoc(current_cmd))
				return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}
