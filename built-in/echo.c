#include "minishell.h"

//ch7al mn -n 7ta -nnnnnn dayza 7ta bzaf dial -nnn -n -nnnnn
static int	handle_n_flags(t_token *tokens)
{
	t_token	*current;
	int		j;
	int		skip_count;

	current = tokens->next;
	skip_count = 0;
	
	while (current)
	{
		// Skip les espaces
		if (current->type == WS)
		{
			current = current->next;
			skip_count++;
			continue;
		}
		
		// Vérifier si c'est un flag -n valide
		if (!current->value || current->value[0] != '-' || current->value[1] != 'n')
			break;
			
		j = 2;
		while (current->value[j])
		{
			if (current->value[j] != 'n')
				return (skip_count + 1); // Pas un flag -n valide
			j++;
		}
		
		current = current->next;
		skip_count++;
	}
	return (skip_count + 1);
}

static void	print_arguments(t_token *tokens, int start_index)
{
	t_token	*current;
	int		count;
	int		first_word;

	current = tokens;
	count = 0;
	first_word = 1;
	
	// Aller au token de départ
	while (current && count < start_index)
	{
		current = current->next;
		count++;
	}
	
	// Imprimer les arguments
	while (current)
	{
		if (current->type == WORD)
		{
			if (!first_word)
				printf(" ");
			printf("%s", current->value);
			first_word = 0;
		}
		current = current->next;
	}
}

void	echo(t_token *tokens)
{
	int	first_arg_index;
	int	no_newline;
	int	original_index;

	if (!tokens || !tokens->value)
		return ;
	
	original_index = 1; // Commencer après "echo"
	first_arg_index = handle_n_flags(tokens);
	
	// Si on a trouvé des flags -n valides
	if (first_arg_index > original_index)
		no_newline = 1;
	else
		no_newline = 0;
	
	print_arguments(tokens, first_arg_index);
	if (!no_newline)
		printf("\n");
}

