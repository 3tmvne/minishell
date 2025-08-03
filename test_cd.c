#include "minishell.h"

// Fonction pour dupliquer l'environnement
char	**dup_env(char **envp)
{
	int		count = 0;
	char	**new_env;
	int		i;

	// Compter le nombre d'entrées
	while (envp[count])
		count++;

	// Allouer le tableau de pointeurs (avec de la place pour de nouvelles entrées)
	new_env = malloc(sizeof(char *) * (count + 10));
	if (!new_env)
		return (NULL);

	// Dupliquer chaque chaîne
	for (i = 0; i < count; i++)
	{
		new_env[i] = ft_strdup(envp[i]);
		if (!new_env[i])
		{
			// Libérer en cas d'erreur
			while (--i >= 0)
				free(new_env[i]);
			free(new_env);
			return (NULL);
		}
	}
	new_env[count] = NULL;
	return (new_env);
}

// Fonction pour libérer l'environnement dupliqué
void	free_env(char **env)
{
	int i = 0;
	if (!env)
		return;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	t_token	*tokens;
	char	**my_env;

	(void)argc;
	(void)argv;

	// Dupliquer l'environnement pour pouvoir le modifier
	my_env = dup_env(envp);
	if (!my_env)
	{
		fprintf(stderr, "Erreur: impossible de dupliquer l'environnement\n");
		return (1);
	}

	// Initialiser PWD avec le répertoire de travail actuel
	char current_dir[4096];
	if (getcwd(current_dir, sizeof(current_dir)))
	{
		// Vérifier si PWD existe déjà dans l'environnement
		int pwd_found = 0;
		for (int i = 0; my_env[i]; i++)
		{
			if (ft_strncmp(my_env[i], "PWD=", 4) == 0)
			{
				pwd_found = 1;
				break;
			}
		}
		
		// Si PWD n'existe pas, l'ajouter manuellement au tableau my_env
		if (!pwd_found)
		{
			// Trouver le premier slot vide
			int i = 0;
			while (my_env[i])
				i++;
			
			// Créer la nouvelle entrée PWD=current_dir
			my_env[i] = malloc(ft_strlen("PWD=") + ft_strlen(current_dir) + 1);
			if (my_env[i])
			{
				// Copie manuelle au lieu de sprintf
				int j = 0, k = 0;
				char *pwd_str = "PWD=";
				while (pwd_str[j])
					my_env[i][k++] = pwd_str[j++];
				j = 0;
				while (current_dir[j])
					my_env[i][k++] = current_dir[j++];
				my_env[i][k] = '\0';
				my_env[i + 1] = NULL; // Marquer la fin
			}
		}
	}

	printf("═══════════════════════════════════════════\n");
	printf("         🎯 TESTEUR FONCTION CD            \n");
	printf("═══════════════════════════════════════════\n");
	printf("Commandes:\n");
	printf("  cd <chemin>     - Changer de répertoire\n");
	printf("  cd              - Aller dans HOME\n");
	printf("  cd -            - Retour à OLDPWD\n");
	printf("  pwd             - Afficher répertoire actuel\n");
	printf("  env             - Voir PWD et OLDPWD\n");
	printf("  exit            - Quitter\n");
	printf("═══════════════════════════════════════════\n\n");

	while (1)
	{
		input = readline("🐚 cd-test> ");
		if (!input)
			break;
		
		if (*input)
			add_history(input);

		if (ft_strncmp(input, "exit", 4) == 0 && input[4] == '\0')
		{
			free(input);
			break;
		}
		else if (ft_strncmp(input, "pwd", 3) == 0 && input[3] == '\0')
		{
			char cwd[4096];
			if (getcwd(cwd, sizeof(cwd)))
				printf("📍 Répertoire actuel: %s\n", cwd);
			else
				perror("pwd");
		}
		else if (ft_strncmp(input, "env", 3) == 0 && input[3] == '\0')
		{
			printf("🌍 Variables d'environnement:\n");
			env_builtin(my_env);
		}
		else
		{
			tokens = tokenizer(input);
			if (!tokens)
			{
				printf("❌ Erreur de tokenisation\n");
				free(input);
				continue;
			}

			if (tokens && tokens->value && ft_strncmp(tokens->value, "cd", 2) == 0 && tokens->value[2] == '\0')
			{
				printf("📂 Exécution de cd...\n");
				cd(tokens, my_env);
				printf("✅ Commande terminée\n");
			}
			else
			{
				printf("⚠️  Ce testeur ne supporte que 'cd', 'pwd', 'env'\n");
			}

			free_tokens(tokens);
		}

		printf("\n");
		free(input);
	}
	
	printf("\n👋 Au revoir !\n");
	free_env(my_env);
	return (0);
}
