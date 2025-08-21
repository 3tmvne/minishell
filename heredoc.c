#include "minishell.h"

extern t_shell_state *g_shell_state;

// Helper to generate unique heredoc filenames
char	*heredoc_filename(int idx)
{
	char	*idx_str;
	char	*name;

	idx_str = ft_itoa(idx);
	name = ft_strjoin(".heredoc_tmp_", idx_str);
	name = ft_strjoin("/tmp/", name);
	return (name);
}

/* Comportement de bash avec les heredocs:
 * - $VAR -> Expansion normale
 * - "$VAR" -> Expansion avec préservation des guillemets doubles
 * - '$VAR' -> Expansion avec préservation des guillemets simples
 */

/* Fonction pour retirer les guillemets du délimiteur pour la comparaison */
char *remove_quotes_from_delimiter(char *delimiter)
{
	char *result;
	int i, j;
	int len = strlen(delimiter);
	
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
	char	*line;
	char	*expanded_line;
	char	*clean_delimiter;
	t_shell_state *shell;

	shell = g_shell_state;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	clean_delimiter = remove_quotes_from_delimiter(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line) // EOF (^D) encountered
		{
			// Display warning message like bash
			write(2, "minishell: warning: here-document delimited by end-of-file (wanted `", 69);
			write(2, clean_delimiter, ft_strlen(clean_delimiter));
			write(2, "')\n", 3);
			break;
		}
		if (ft_strcmp(line, clean_delimiter) == 0)
			break ;
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
	close(fd);
	exit(0);
}

char	*handle_heredoc_file(char *delimiter, int idx, int has_quotes) //! minishell$> echo dgbgd | << l
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
		/* Traitement des heredocs - le délimiteur reste tel quel */
		child(fd, delimiter, !has_quotes);
	}
	else
	{
		close(fd);
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
		{
			unlink(filename);
			// Si interrompu par ^C, propager le signal
			if (WTERMSIG(status) == SIGINT)
			{
				if (g_shell_state)
					g_shell_state->last_exit_status = 130;
				return (NULL); // Retourner NULL pour indiquer l'interruption
			}
		}
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
