/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:38:39 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:17:15 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	save_fds(t_cmd *cmd)
{
	cmd->save_stdin = dup(0);
	if (cmd->save_stdin == -1)
		return (1);
	cmd->save_stdout = dup(1);
	if (cmd->save_stdout == -1)
		return (1);
	return (0);
}

int	restor_fd(t_cmd *cmd)
{
	if (cmd->save_stdin != -1)
		dup2(cmd->save_stdin, 0);
	if (cmd->save_stdout != -1)
		dup2(cmd->save_stdout, 1);
	else
		return (1);
	return (0);
}

int	open_files(t_token *red)
{
	int	fd;

	fd = 0;
	if (red->type == HEREDOC && (!red->value || red->value[0] == '\0'))
		return (0);
	if (red->type == REDIR_IN || red->type == HEREDOC)
		fd = open(red->value, O_RDONLY);
	else if (red->type == REDIR_OUT)
		fd = open(red->value, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else if (red->type == APPEND)
		fd = open(red->value, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("open");
		return (-1);
	}
	return (fd);
}

int	dup_fd(t_token *red, int fd)
{
	int	rd;

	rd = 0;
	if (red->type == HEREDOC && (!red->value || red->value[0] == '\0'))
		return (0);
	if (red->type == REDIR_IN || red->type == HEREDOC)
		rd = dup2(fd, 0);
	else if (red->type == REDIR_OUT || red->type == APPEND)
		rd = dup2(fd, 1);
	if (rd == -1)
	{
		return (-1);
	}
	return (fd);
}

int	redirection(t_cmd *cmd)
{
	int		fd;
	t_token	*red;

	fd = 0;
	if (save_fds(cmd))
		return (1);
	red = cmd->redirections;
	while (red)
	{
		if (!(red->type == HEREDOC && (!red->value || red->value[0] == '\0')))
		{
			fd = open_files(red);
			if (fd == -1)
				return (restor_fd(cmd), 1);
			if (dup_fd(red, fd) == -1)
				return (restor_fd(cmd), 1);
			if (red->type == HEREDOC)
				unlink(red->value);
		}
		red = red->next;
	}
	return (0);
}
