/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:40:42 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/26 20:31:49 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*remove_quotes_from_delimiter(char *delimiter)
{
	char	*result;
	int		len;
	int		i;
	int		j;

	len = ft_strlen(delimiter);
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

void	handle_heredoc_eof(char *delimiter)
{
	ft_putstr_fd("bash: warning: here-document at ", 2);
	ft_putstr_fd("line delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(delimiter, 2);
	ft_putstr_fd("')\n", 2);
	free_gc_all();
	exit(0);
}

void	process_heredoc_line(int fd, char *line, int should_expand,
							t_shell_state *shell)
{
	char	*expanded_line;

	if (shell && should_expand)
	{
		expanded_line = expand_heredoc_line(line, shell);
		if (expanded_line)
			write(fd, expanded_line, ft_strlen(expanded_line));
		else
			write(fd, line, ft_strlen(line));
	}
	else
		write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

void	child(int fd, char *delimiter, int should_expand)
{
	char			*line;
	char			*clean_delimiter;
	t_shell_state	*shell;

	shell = get_shell_state(NULL);
	signal(SIGINT, handler_signal_heredoc);
	clean_delimiter = remove_quotes_from_delimiter(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
			handle_heredoc_eof(delimiter);
		if (strcmp(line, clean_delimiter) == 0)
		{
			free(line);
			break ;
		}
		process_heredoc_line(fd, line, should_expand, shell);
		free(line);
	}
	close(fd);
	free_gc_all();
	exit(0);
}

char	*handle_heredoc_file(char *delimiter, int idx, t_quote_type quote_type)
{
	char	*filename;
	pid_t	pid;

	int (should_expand), (fd), (status);
	should_expand = (quote_type == NQUOTES);
	filename = heredoc_filename(idx);
	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		child(fd, delimiter, should_expand);
	else
	{
		close(fd);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			get_shell_state(NULL)->last_exit_status = WEXITSTATUS(status);
		if (WEXITSTATUS(status) == 130)
		{
			unlink(filename);
			get_shell_state(NULL)->last_exit_status = 130;
			return (NULL);
		}
	}
	return (filename);
}
