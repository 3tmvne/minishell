#include "minishell.h"

// void	redir_in(char *file)
// {
// 	int		fd;

// 	fd = open(file, O_RDONLY);
// 	if (fd < 0)
// 	{
// 		perror("open");
// 		return;
// 	}
// 	if (dup2(fd, STDIN_FILENO) == -1)
// 	{
// 		perror("dup2");
// 		close(fd);
// 		return;
// 	}
// 	close(fd);
// }

// void	redir_out(char *file)
// {
// 	int		fd;

// 	fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
// 	if (fd < 0)
// 	{
// 		perror("open");
// 		return;
// 	}
// 	if (dup2(fd, STDOUT_FILENO) == -1)
// 	{
// 		perror("dup2");
// 		close(fd);
// 		return;
// 	}
// 	close(fd);
// }

// void	redir_append(char *file)
// {
// 	int		fd;

// 	fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
// 	if (fd < 0)
// 	{
// 		perror("open");
// 		return;
// 	}
// 	if (dup2(fd, STDOUT_FILENO) == -1)
// 	{
// 		perror("dup2");
// 		close(fd);
// 		return;
// 	}
// 	close(fd);
// }

// int	apply_redirection(t_token *redir)
// {
// 	int i = 0;
// 	char *filename;

// 	if (!redir)
// 		return (0); // Pas de redirection = succès, pas d'erreur
// 	if (redir->type == REDIR_IN)
// 		redir_in(redir->value);
// 	else if (redir->type == REDIR_OUT)
// 		redir_out(redir->value);
// 	else if (redir->type == APPEND)
// 		redir_append(redir->value);
// 	while (redir->type == HEREDOC)
// 	{
// 		if (redir->type == HEREDOC)
// 		{
// 			filename = handle_heredoc_file(redir->value, i);
// 		}
// 		if (redir->next == NULL)
// 		{
// 			redir_in(filename);
// 			break;
// 		}
// 		redir = redir->next;
// 		i++;
// 	}
// 	return 0;
// }
//--------------------------------------------//

int save_fds(t_cmd *cmd)
{
	cmd->save_stdin = dup(0);
	if (cmd->save_stdin == -1)
		return (1);
	cmd->save_stdout = dup(1);
	if (cmd->save_stdout == -1)
		return (1);
	return (0);
}

int restor_fd(t_cmd *cmd)
{
	if (cmd->save_stdin != -1)
		dup2(cmd->save_stdin, 0);
	if (cmd->save_stdout != -1)
		dup2(cmd->save_stdout, 1);
	else 
		return (1);
	return (0);
}

int open_files(t_token *red)
{
	int		fd;

	fd = 0;
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

int dup_fd(t_token *red, int fd)
{
	int rd;

	rd = 0;
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

int redirection(t_cmd *cmd)
{
	int fd;
	t_token *red;

	fd = 0;
	red = cmd->redirections;
	if (save_fds(cmd))
		return (1);
	while(red)
	{
		fd = open_files(red);
		if (fd == -1)
			return (restor_fd(cmd), 1);
		if (dup_fd(red, fd) == -1)
			return (restor_fd(cmd), 1);
		red = red->next;
	}
	return (0);
}
