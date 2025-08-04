#include "minishell.h"

int	handle_heredoc_file(char *delimiter)
{
	int		fd;
	int		fd2;
	pid_t	pid;
	char	*line;
	int		status;
	
	fd = open(".heredoc_test_file", O_CREAT | O_WRONLY | O_TRUNC, 0644);
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
	if (pid == 0) //* child
	{
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
	else //* parent
	{
		close(fd); //? close write-end
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status)) //? true if child terminated due to a signal
		unlink(".heredoc_test_file");   //? clean up if Ctrl-C
	}
	fd2 = open(".heredoc_test_file", O_RDONLY);
	return fd2; //? return read-end of heredoc file
}

// int	main(void)
// {

// 	handle_heredoc_file("a");
// 	handle_heredoc_file("b");
// 	handle_heredoc_file("c");

// 	printf("\n--- Content written to heredoc file ---\n");

// 	int		fd = open(".heredoc_test_file", O_RDONLY);
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
