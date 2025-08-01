#include "minishell.h"

void	handle_heredoc_file(const char *delimiter, const char *file_name)
{
	int		fd;
	pid_t	pid;
	char	*line;
	int		status;

	fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
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
			if (!line || strcmp(line, delimiter) == 0)
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
		if (WIFSIGNALED(status)) //? Did the child exit because of a signal?
			unlink(file_name); //? clean up if Ctrl-C
	}
}

// int	main(void)
// {
// 	const char	*delimiter = "EOF";
// 	const char	*file_name = ".heredoc_test_file";

// 	handle_heredoc_file(delimiter, file_name);

// 	printf("\n--- Content written to %s ---\n", file_name);

// 	int		fd = open(file_name, O_RDONLY);
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
// 	unlink(file_name); // Clean up
// 	return (0);
// }
