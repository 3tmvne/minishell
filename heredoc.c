#include "minishell.h"

void child(int fd, char *delimiter)
{
	char	*line;

	signal(SIGINT, SIG_DFL); //? Reset Ctrl+C to kill process
	while (1)
	{
		line = readline("> ");
		if (!line || strcmp(line, delimiter) == 0) //? Ctrl+D or delimiter
			break ;
		write(fd, line, strlen(line));
		write(fd, "\n", 1);
	}
	close(fd);
	exit(0);
}

int	handle_heredoc_file(char *delimiter)
{
	int		fd[2];
	pid_t	pid;
	int		status;
	
	fd[0] = open(".heredoc_test_file", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd[0] == -1)
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
	if (pid == 0) //* child
		child(fd[0], delimiter);
	else //* parent
	{
		close(fd[0]); //? close write-end
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status)) //? true if child terminated due to a signal
			unlink(".heredoc_test_file");   //? clean up if Ctrl-C
	}
	fd[1] = open(".heredoc_test_file", O_RDONLY);
	return fd[1]; //? return read-end of heredoc file
}

// int	main(void)
// {

// 	int fd = handle_heredoc_file("a");

// 	printf("\n--- Content written to heredoc file ---\n");

// 	if (fd == -1)
// 	{
// 		perror("open for reading");
// 		return (1);
// 	}

// 	char	buffer[1024];
// 	ssize_t	bytes;

// 	while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0)
// 	{
// 		buffer[bytes] = '\0';
// 		write(STDOUT_FILENO, buffer, bytes);
// 	}

// 	close(fd);
// 	unlink(".heredoc_test_file"); // Clean up
// 	return (0);
// }
