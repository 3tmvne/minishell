/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:40:42 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/24 19:42:30 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*heredoc_filename(int idx)
{
	char	*name;
	char	*idx_str;

	idx_str = ft_itoa(idx);
	name = ft_strjoin(".heredoc_tmp_", idx_str);
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

	shell = get_shell_state(NULL);
	signal(SIGINT, handler_signal_heredoc);
	clean_delimiter = remove_quotes_from_delimiter(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("bash: warning: here-document at line delimited by end-of-file (wanted `", 2);
			ft_putstr_fd(delimiter, 2);
			ft_putstr_fd("')\n", 2);
			free_gc_all();
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
	free_gc_all();
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
	signal(SIGINT, SIG_IGN);
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
		if (WIFEXITED(status))
		{
			get_shell_state(NULL)->last_exit_status = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			unlink(filename);
			get_shell_state(NULL)->last_exit_status = WTERMSIG(status) + 128;
			return (NULL);
		}
	}
	return (filename);
}
