#include "minishell.h"

//ch7al mn -n 7ta -nnnnnn dayza 7ta bzaf dial -nnn -n -nnnnn
static int	handle_n_flags(char **args)
{
	int		i;
	int		j;

	i = 1; // Start from index 1 (skip "echo")
	
	while (args[i])
	{
		// Vérifier si c'est un flag -n valide
		if (!args[i] || args[i][0] != '-' || args[i][1] != 'n')
			break;
			
		j = 2;
		while (args[i][j])
		{
			if (args[i][j] != 'n')
				return (i); // Pas un flag -n valide
			j++;
		}
		
		i++;
	}
	return (i);
}

static void	print_arguments(char **args, int start_index)
{
	int		i;
	int		first_word;

	i = start_index;
	first_word = 1;
	
	// Imprimer les arguments
	while (args[i])
	{
		if (!first_word)
			printf(" ");
		printf("%s", args[i]);
		first_word = 0;
		i++;
	}
}

void	echo(t_cmd *cmd)
{
	int	first_arg_index;
	int	no_newline;
	int	original_index;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	
	original_index = 1; // Commencer après "echo"
	first_arg_index = handle_n_flags(cmd->args);
	
	// Si on a trouvé des flags -n valides
	if (first_arg_index > original_index)
		no_newline = 1;
	else
		no_newline = 0;
	
	print_arguments(cmd->args, first_arg_index);
	if (!no_newline)
		printf("\n");
}

