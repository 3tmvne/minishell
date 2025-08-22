#include "minishell.h"

// Fonction pour récupérer une variable d'environnement
char	*get_env_value(const char *name, char **env)
{
	int	i;
	int	name_len;

	if (!name || !env)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (&env[i][name_len + 1]);
		i++;
	}
	return (NULL);
}

// Fonction pour mettre à jour une variable d'environnement

void	update_env_value(const char *name, const char *value, char **env)
{
	int		i;
	int		name_len;
	char	*new_var;

	if (!name || !value || !env)
		return ;
	name_len = ft_strlen(name);
	// Créer la nouvelle chaîne "NAME=value"
	new_var = create_env_string(name, value);
	if (!new_var)
		return ;
	// Chercher si la variable existe déjà
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
		{
			// Remplacer la variable existante
			free(env[i]);
			env[i] = new_var;
			return ;
		}
		i++;
	}
	// Si on arrive ici, la variable n'existe pas, l'ajouter à la fin
	// On assume qu'il y a assez d'espace (alloué avec +10 dans dup_env)
	env[i] = new_var;
	env[i + 1] = NULL;
}

void	cd(t_cmd *cmd, t_env **env)
{
	char	*path;
	char	*newpwd;

	char *oldpwd = getcwd(NULL, 0); // getcwd alloue automatiquement
	if (!oldpwd)
	{
		perror("getcwd failed");
		return ;
	}
	// 1. Récupérer argument (le premier argument après "cd")
	if (!cmd->args[1])
	{
		// 2. Gérer cas pas d'argument (aller dans HOME)
		path = get_env_value_list(*env, "HOME");
		if (!path)
		{
			fprintf(stderr, "cd: HOME not set\n");//!!!!!!!!!
			free(oldpwd);
			return ;
		}
	}
	else
		path = cmd->args[1];
	// Vérifier que path n'est pas NULL avant de continuer
	if (!path)
	{
		fprintf(stderr, "cd: invalid path\n");//!!!!!!!!!
		free(oldpwd);
		return ;
	}
	// 3. Appeler chdir()
	if (chdir(path) != 0)
	{
		// 4. Gérer erreurs
		perror("cd");
		free(oldpwd);
		return ;
	}
	// 5. Mettre à jour PWD et OLDPWD
	newpwd = getcwd(NULL, 0);
	if (newpwd)
	{
		set_env_var(env, "PWD", newpwd);
		set_env_var(env, "OLDPWD", oldpwd);
		free(newpwd);
	}
	else
	{
		fprintf(stderr, "cd: error updating PWD\n");//!!!!!!!!!
	}
	free(oldpwd);
}
