#include "minishell.h"

int	built_cmd(t_cmd *cmd, t_shell_state *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!ft_strcmp(cmd->args[0], "echo"))
	{
		echo(cmd);
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "export"))
		shell->last_exit_status = export_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		shell->last_exit_status = unset_builtin(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "pwd"))
	{
		pwd_builtin();
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "env"))
	{
		env_builtin(shell->env);
		shell->last_exit_status = 0;
	}
	else if (!ft_strcmp(cmd->args[0], "cd"))
		cd(cmd, &shell->env);
	else if (!ft_strcmp(cmd->args[0], "exit"))
		exit_builtin(cmd, shell->last_exit_status);
	return (1);
}
// Helper: affiche une erreur de commande externe et quitte le processus fils

static char	*check_absolute_path(const char *cmd, int *err)
{
	struct stat st;
	
	// D'abord vérifier si le fichier existe
	if (access(cmd, F_OK) != 0)
	{
		*err = 124; // Code spécial pour "No such file or directory" (chemins absolus)
		return (NULL);
	}
	// Ensuite vérifier si c'est un répertoire
	if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
	{
		*err = 125; // Code spécial pour "Is a directory"
		return (NULL);
	}
	// Enfin vérifier les permissions d'exécution
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	*err = 126; // Permission denied pour les fichiers
	return (NULL);
}

static char	*search_in_path(const char *cmd, char **paths, int *err)
{
	int		i;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		if (!ft_strcmp(cmd, ".") || !ft_strcmp(cmd, ".."))
			break;
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(full_path, cmd);
		if (access(full_path, X_OK) != 0 && access(full_path, F_OK) == 0)
		{
			*err = 126;
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
			return (full_path);
		i++;
	}
	*err = 127;
	return (NULL);
}

char	*find_command_path(const char *cmd, t_env *env, int *err)
{
	char	*path;
	char	**paths;

	*err = 0;
	if (ft_strchr(cmd, '/'))
		return (check_absolute_path(cmd, err));
	path = get_env_value_list(env, "PATH");
	if (!path)
	{
		// Quand PATH n'est pas défini, bash cherche seulement dans le répertoire courant
		// mais traite les échecs comme "command not found"
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		*err = 127; // command not found quand PATH n'existe pas
		return (NULL);
	}
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(cmd, paths, err));
}

static void	print_and_exit_external_error(const char *cmd, int err)
{
    char *msg;
    char *tmp;

    if (err == 127)
    {
        tmp = ft_strjoin("minishell: ", cmd);
        msg = ft_strjoin(tmp, ": command not found\n");
        write(2, msg, ft_strlen(msg));
        exit(127);
    }
    if (err == 126)
    {
        tmp = ft_strjoin("minishell: ", cmd);
        msg = ft_strjoin(tmp, ": Permission denied\n");
        write(2, msg, ft_strlen(msg));
        exit(126);
    }
    if (err == 125)
    {
        tmp = ft_strjoin("minishell: ", cmd);
        msg = ft_strjoin(tmp, ": Is a directory\n");
        write(2, msg, ft_strlen(msg));
        exit(126); // bash retourne 126 pour "Is a directory"
    }
    if (err == 124)
    {
        tmp = ft_strjoin("minishell: ", cmd);
        msg = ft_strjoin(tmp, ": No such file or directory\n");
        write(2, msg, ft_strlen(msg));
        exit(127); // bash retourne 127 pour "No such file or directory"
    }
}

void	extern_cmd(t_cmd *cmd, t_shell_state *shell)
{
	char	**env_array;
	char	*path;
	int		err;

	env_array = env_to_array(shell->env);
	if (cmd->redirections)
		redirection(cmd);
	if (!env_array)
		exit(EXIT_FAILURE);
	path = find_command_path(cmd->args[0], shell->env, &err);
	if (!path || err != 0)
		print_and_exit_external_error(cmd->args[0], err);
	if (execve(path, cmd->args, env_array) == -1)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
}

int	is_built_cmd(t_cmd *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	
	if (!ft_strcmp(cmd->args[0], "echo") || !ft_strcmp(cmd->args[0], "cd")
		|| !ft_strcmp(cmd->args[0], "unset") || !ft_strcmp(cmd->args[0], "env")
		|| !ft_strcmp(cmd->args[0], "export") || !ft_strcmp(cmd->args[0], "exit")
		|| !ft_strcmp(cmd->args[0], "pwd"))
		return (1);
	return (0);
}

void	execute(t_pipeline *line, t_shell_state *shell)
{
	if (!line || !shell)
		return;
	
	if (!line->commands->args || !line->commands->args[0])
	{
		if (line->commands->redirections)
		{
			if (redirection(line->commands))
				return ;
			restor_fd(line->commands);
		}
		shell->last_exit_status = 0; // Empty command succeeds with status 0
		return;
	}
	if (line->commands->args[0][0] == '\0')
	{
		write(2, "minishell: : command not found\n", 32);
		shell->last_exit_status = 127;
		return;
	}
	if (is_built_cmd(line->commands) && line->cmd_count == 1)
	{
		if (line->commands->redirections)
		{
			if (redirection(line->commands))
				return ;
		}
		built_cmd(line->commands, shell);
		if (line->commands->redirections)
			restor_fd(line->commands);
	}
	else
		pipes(line, shell);		
}
