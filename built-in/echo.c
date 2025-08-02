#include "minishell.h"

//ch7al mn -n 7ta -nnnnnn dayza 7ta bzaf dial -nnn -n -nnnnn
static int	handle_n_flags(t_token *tokens)
{
	t_token	*current;
	int		j;
	int		count;

	current = tokens->next;
	count = 1;
	while (current)
	{
		if (!current->value || current->value[0] != '-' || current->value[1] != 'n')
			break;
		j = 2;
		while (current->value[j])
		{
			if (current->value[j] != 'n')
				return (count);
			j++;
		}
		current = current->next;
		count++;
	}
	return (count);
}

static void	print_arguments(t_token *tokens, int start_index)
{
	t_token	*current;
	int		count;

	current = tokens;
	count = 0;
	while (current && count < start_index)
	{
		current = current->next;
		count++;
	}
	while (current)
	{
		if (current->type == WORD)
		{
			printf("%s", current->value);
			if (current->next && current->next->type == WORD)
				printf(" ");
		}
		current = current->next;
	}
}

void	echo(t_token *tokens)
{
	int	first_arg_index;
	int	no_newline;

	if (!tokens || !tokens->value)
		return ;
	first_arg_index = handle_n_flags(tokens);
	if (first_arg_index > 1)
		no_newline = 1;
	else
		no_newline = 0;
	print_arguments(tokens, first_arg_index);
	if (!no_newline)
		printf("\n");
}

