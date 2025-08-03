#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	t_token	*tokens;

	(void)argc;
	(void)argv;

	printf("═══════════════════════════════════════════\n");
	printf("         🎯 TESTEUR FONCTION CD (ENV)      \n");
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
		input = readline("🐚 cd-env> ");
		if (!input)
			break;
		
		if (*input)
			add_history(input);

		if (strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
		else if (strcmp(input, "pwd") == 0)
		{
			char cwd[4096];
			if (getcwd(cwd, sizeof(cwd)))
				printf("📍 Répertoire actuel: %s\n", cwd);
			else
				perror("pwd");
		}
		else if (strcmp(input, "env") == 0)
		{
			char *pwd = get_env_value("PWD", envp);
			char *oldpwd = get_env_value("OLDPWD", envp);
			printf("🌍 PWD=%s\n", pwd ? pwd : "(non défini)");
			printf("🔄 OLDPWD=%s\n", oldpwd ? oldpwd : "(non défini)");
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

			if (tokens && tokens->value && strcmp(tokens->value, "cd") == 0)
			{
				printf("📂 Exécution de cd avec environnement...\n");
				cd(tokens, envp);
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
	return (0);
}
