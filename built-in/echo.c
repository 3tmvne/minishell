#include "minishell.h"

/**
 * Vérifie si un argument est un flag -n valide
 */
static int	is_valid_n_flag(const char *value)
{
	int	i;

	if (!value || value[0] != '-' || value[1] != 'n')
		return (0);
		
	i = 2;
	while (value[i])
	{
		if (value[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

void	echo(t_cmd *cmd)
{
	int		i;
	int		no_newline;
	int		first_arg;

	if (!cmd || !cmd->args)
		return;
	
	no_newline = 0;
	i = 1; // Commencer à l'index 1 pour ignorer "echo"
	
	// Traiter les flags -n
	while (cmd->args[i] && is_valid_n_flag(cmd->args[i]))
	{
		no_newline = 1;
		i++;
	}
	
	// Imprimer tous les arguments
	first_arg = 1;
	while (cmd->args[i])
	{
		if (!first_arg)
			printf(" ");
		printf("%s", cmd->args[i]);
		first_arg = 0;
		i++;
	}
	
	if (!no_newline)
		printf("\n");
}

