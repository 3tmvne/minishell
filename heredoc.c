#include "minishell.h"

extern t_shell_state *g_shell_state;

// Helper to generate unique heredoc filenames
char	*heredoc_filename(int idx)
{
	char	*name;

	name = ft_strdup(".heredoc_tmp_");
	name = ft_strjoin(name, ft_itoa(idx));
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

/* Fonction spéciale pour l'expansion dans les heredocs qui respecte les règles de bash */
char *expand_heredoc_line(char *line, t_shell_state *shell)
{
    char *result;
    int i, j;
    int len = strlen(line);
    char *expanded_var;
    char var_name[256];
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    /* Allouer suffisamment d'espace pour le résultat */
    result = ft_malloc((len * 2) + 1); /* Estimation généreuse */
    if (!result)
        return (NULL);
    
    i = 0;
    j = 0;
    
    while (i < len)
    {
        /* Gérer les guillemets mais les conserver dans la sortie */
        if (line[i] == '\'')
        {
            result[j++] = line[i++];
            in_single_quotes = !in_single_quotes;
            continue;
        }
        else if (line[i] == '"')
        {
            result[j++] = line[i++];
            in_double_quotes = !in_double_quotes;
            continue;
        }
        
        /* Expansion des variables (même à l'intérieur des quotes comme dans bash) */
        if (line[i] == '$' && i + 1 < len && (isalpha(line[i+1]) || line[i+1] == '_'))
        {
            int var_start = i + 1;
            int var_len = 0;
            
            /* Capturer le nom de la variable */
            while (var_start + var_len < len && 
                   (isalnum(line[var_start + var_len]) || line[var_start + var_len] == '_'))
            {
                var_len++;
            }
            
            strncpy(var_name, line + var_start, var_len);
            var_name[var_len] = '\0';
            
            /* Récupérer la valeur de la variable */
            expanded_var = get_env_value_list(shell->env, var_name);
            
            if (expanded_var)
            {
                strcpy(result + j, expanded_var);
                j += strlen(expanded_var);
            }
            
            i = var_start + var_len;
        }
        /* Gérer le cas spécial $? pour le code de sortie */
        else if (line[i] == '$' && i + 1 < len && line[i+1] == '?')
        {
            char *status_str = ft_itoa(shell->last_exit_status);
            if (status_str)
            {
                strcpy(result + j, status_str);
                j += strlen(status_str);
            }
            i += 2; /* Sauter $? */
        }
        else
        {
            /* Copier le caractère tel quel */
            result[j++] = line[i++];
        }
    }
    
    result[j] = '\0';
    return (result);
}

void	child(int fd, char *delimiter)
{
	char	*line;
	char	*expanded_line;
	char	*clean_delimiter;
	t_shell_state *shell;

	shell = g_shell_state;
	/* Configuration pour gérer les signaux dans le processus enfant */
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	
	/* Retirer les guillemets du délimiteur pour la comparaison */
	clean_delimiter = remove_quotes_from_delimiter(delimiter);
	
	while (1)
	{
		line = readline("> ");
		if (!line || strcmp(line, clean_delimiter) == 0)
			break ;
		
		/* Expansion des variables dans le heredoc selon les règles de bash */
		if (shell)
		{
			/* Utiliser notre fonction spéciale pour les heredocs */
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
		/* Traitement des heredocs - le délimiteur reste tel quel */
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
