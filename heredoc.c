#include "minishell.h"

// Helper to generate unique heredoc filenames
char	*heredoc_filename(int idx)
{
	char	*name;

	name = ft_strdup(".heredoc_tmp_");
	name = ft_strjoin(name, ft_itoa(idx));
	name = ft_strjoin("/tmp/", name);
	return (name);
}

void	child(int fd, char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || strcmp(line, delimiter) == 0)
			break ;
		write(fd, line, strlen(line));
		write(fd, "\n", 1);
	}
	close(fd);
	exit(0);
}

char	*handle_heredoc_file(char *delimiter, int idx)
{
	char	*filename;
	int		fd;
	pid_t	pid;
	int		status;

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
	{
		child(fd, delimiter);
	}
	else
	{
		close(fd);
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
			unlink(filename);
	}
	return (filename);
}

// int	main(void)
// {
// 	int		fd_a;
// 	int		fd_b;
// 	int		fd_c;
// 	char	buffer[1024];
// 	ssize_t	bytes;

// 	fd_a = handle_heredoc_file_unique("a", 0);
// 	fd_b = handle_heredoc_file_unique("b", 1);
// 	fd_c = handle_heredoc_file_unique("c", 2);
// 	printf("\n--- Content written to heredoc files ---\n");
// 	printf("Contents of heredoc a:\n");
// 	while ((bytes = read(fd_a, buffer, sizeof(buffer) - 1)) > 0)
// 	{
// 		buffer[bytes] = '\0';
// 		write(STDOUT_FILENO, buffer, bytes);
// 	}
// 	close(fd_a);
// 	unlink(".heredoc_tmp_0");
// 	printf("Contents of heredoc b:\n");
// 	while ((bytes = read(fd_b, buffer, sizeof(buffer) - 1)) > 0)
// 	{
// 		buffer[bytes] = '\0';
// 		write(STDOUT_FILENO, buffer, bytes);
// 	}
// 	close(fd_b);
// 	unlink(".heredoc_tmp_1");
// 	printf("Contents of heredoc c:\n");
// 	while ((bytes = read(fd_c, buffer, sizeof(buffer) - 1)) > 0)
// 	{
// 		buffer[bytes] = '\0';
// 		write(STDOUT_FILENO, buffer, bytes);
// 	}
// 	close(fd_c);
// 	unlink(".heredoc_tmp_2");
// 	return (0);
// }
