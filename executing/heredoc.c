/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:40:42 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/22 17:25:24 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_shell_state	*g_shell_state;

char	*heredoc_filename(int idx)
{
	char	*name;
	char	*idx_str;

	idx_str = ft_itoa(idx);
	name = ft_strjoin("/tmp/.heredoc_tmp_", idx_str);
	return (name);
}

char	*remove_quotes_from_delimiter(char *delimiter)
{
	char	*result;
	int		len;
	int		i;
	int		j;

	len = strlen(delimiter);
	result = ft_malloc(len + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (delimiter[i] != '"' && delimiter[i] != '\'')
		{
			result[j++] = delimiter[i];
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

void	child(int fd, char *delimiter, int should_expand)
{
	char			*line;
	char			*expanded_line;
	char			*clean_delimiter;
	t_shell_state	*shell;

	shell = g_shell_state;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	clean_delimiter = remove_quotes_from_delimiter(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("bash: warning: here-document at line ", 2);
			ft_putstr_fd("delimited by end-of-file (wanted `", 2);
			ft_putstr_fd(delimiter, 2);
			ft_putstr_fd("')\n", 2);
			exit(0);
		}
		if (strcmp(line, clean_delimiter) == 0)
			break ;
		if (shell && should_expand)
		{
			expanded_line = expand_heredoc_line(line, shell);
			if (expanded_line)
			{
				write(fd, expanded_line, strlen(expanded_line));
			}
			else
				write(fd, line, strlen(line));
		}
		else
			write(fd, line, strlen(line));
		write(fd, "\n", 1);
	}
	close(fd);
	exit(0);
}

char	*handle_heredoc_file(char *delimiter, int idx, t_quote_type quote_type)
{
	char	*filename;
	int		fd;
	pid_t	pid;
	int		status;
	int		should_expand;

	should_expand = (quote_type == NQUOTES);
	filename = heredoc_filename(idx);
	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
		child(fd, delimiter, should_expand);
	else
	{
		close(fd);
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			unlink(filename);
			if (g_shell_state)
				g_shell_state->last_exit_status = 130;
			return (NULL); // Return NULL to indicate interruption
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		{
			unlink(filename);
			if (g_shell_state)
				g_shell_state->last_exit_status = 130;
			return (NULL); // Return NULL to indicate interruption
		}
		if (WIFSIGNALED(status))
			unlink(filename);
	}
	return (filename);
}
