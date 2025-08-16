#include "minishell.h"

void	redir_in(char *file)
{
	int		fd;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return;
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return;
	}
	close(fd);
}

void	redir_out(char *file)
{
	int		fd;

	fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("open");
		return;
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return;
	}
	close(fd);
}

void	redir_append(char *file)
{
	int		fd;

	fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd < 0)
	{
		perror("open");
		return;
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return;
	}
	close(fd);
}

int	apply_redirection(t_token *redir)
{
	int		fd = 0;
	
	if (redir->type == REDIR_IN)
		redir_in(redir->value);
	else if (redir->type == REDIR_OUT)
		redir_out(redir->value);
	else if (redir->type == APPEND)
		redir_append(redir->value);
	else if (redir->type == HEREDOC)
		fd = handle_heredoc_file(redir->value);
	return (fd);
}
