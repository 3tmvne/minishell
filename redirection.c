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

void	append(char *file)
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

void	apply_redirection(t_token *redir)
{
	int		fd;
	t_token	*file_token;
	
	file_token = redir->next;
	if (file_token->type == WS && file_token->next)
		file_token = file_token->next;

	if (redir->type == REDIR_IN)
		redir_in(file_token->value);
	else if (redir->type == REDIR_OUT)
		redir_out(file_token->value);
	else if (redir->type == APPEND)
		append(file_token->value);
	else if (redir->type == HEREDOC)
		fd = handle_heredoc_file(file_token->value);
}
