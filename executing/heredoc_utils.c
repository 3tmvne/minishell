/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:19:06 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:34:48 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	heredoc(t_cmd *cmd)
{
	static int	i;
	t_token		*red;
	char		*heredoc_file;

	red = cmd->redirections;
	while (red)
	{
		if (red->type == HEREDOC)
		{
			heredoc_file = handle_heredoc_file(red->value, i, red->quote);
			i++;
			if (!heredoc_file)
				return (1);
			if (get_shell_state(NULL)->last_exit_status == 130)
				return (1);
			red->value = ft_strdup(heredoc_file);
		}
		red = red->next;
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

char	*heredoc_filename(int idx)
{
	char	*name;
	char	*idx_str;

	idx_str = ft_itoa(idx);
	name = ft_strjoin(".heredoc_tmp_", idx_str);
	return (name);
}
