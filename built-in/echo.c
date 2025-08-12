#include "minishell.h"

/**
 * Vérifie si un token est un flag -n valide
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

/**
 * Trouve le premier token qui n'est pas un flag -n
 */
static t_token	*skip_n_flags(t_token *tokens, int *no_newline)
{
	t_token	*current;

	current = tokens->next; // Skip "echo"
	*no_newline = 0;
	
	while (current && current->type == WORD && is_valid_n_flag(current->value))
	{
		*no_newline = 1;
		current = current->next;
	}
	
	return (current);
}

void	echo(t_token *tokens, t_shell_state *shell)
{
	t_token	*current;
	int		no_newline;
	int		first_word;

	if (!tokens)
	{
		shell->last_exit_status = 0;
		return;
	}
	
	// Traiter les flags -n et trouver le premier argument
	current = skip_n_flags(tokens, &no_newline);
	
	// Imprimer tous les arguments
	first_word = 1;
	while (current && current->type == WORD)
	{
		printf("%s", current->value);
		first_word = 0;
		current = current->next;
	}
	
	if (!no_newline)
		printf("\n");
		
	shell->last_exit_status = 0;
}

