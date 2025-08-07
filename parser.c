#include "minishell.h"

t_cmd	*create_new_command(void)
{
	t_cmd	*cmd;

	cmd = calloc(1, sizeof(t_cmd));
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}
void	add_argument(t_cmd *cmd, char *arg)
{
	int		count;
	char	**new_args;
	int		i;

	count = 0;
	// Count existing arguments
	if (cmd->args)
	{
		while (cmd->args[count])
			count++;
	}
	// Allocate new array
	new_args = malloc(sizeof(char *) * (count + 2));
	if (cmd->args)
	{
		i = 0;
		while (i < count)
		{
			new_args[i] = cmd->args[i];
			i++;
		}
		free(cmd->args);
	}
	new_args[count] = arg;
	new_args[count + 1] = NULL;
	cmd->args = new_args;
}

void	add_redirection(t_cmd *cmd, t_token_type type, char *filename)
{
	t_token	*redir;
	t_token	*last;

	redir = calloc(1, sizeof(t_token));
	redir->type = type;
	redir->value = filename;
	redir->quote = NQUOTES; // Default quote type
	redir->next = NULL;
	redir->prev = NULL;
	// Add to end of redirection list
	if (!cmd->redirections)
	{
		cmd->redirections = redir;
	}
	else
	{
		last = cmd->redirections;
		while (last->next)
			last = last->next;
		last->next = redir;
	}
}

t_pipeline	*parse(t_token *tokens)
{
	t_pipeline	*pipeline;
	t_cmd		*current_cmd;
	t_token		*token;
	t_token		*next;

	pipeline = calloc(1, sizeof(t_pipeline));
	current_cmd = NULL;
	token = tokens;
	// Start with first command
	current_cmd = create_new_command();
	pipeline->commands = current_cmd;
	pipeline->cmd_count = 1;
	while (token)
	{
		if (token->type == WORD)
		{
			// Add word as argument to current command
			add_argument(current_cmd, token->value);
		}
		else if (token->type == HEREDOC)
			add_redirection(current_cmd, HEREDOC, token->value); //! heredoc handling
		else if (token->type == REDIR_OUT || token->type == REDIR_IN
			|| token->type == APPEND)
		{
			// Skip any WS tokens after redirection
			next = token->next;
			while (next && next->type == WS)
				next = next->next;
			// Only add if next is a WORD (filename)
			if (next && next->type == WORD)
			{
				add_redirection(current_cmd, token->type, next->value);
				token = next; // Skip the filename token
			}
		}
		else if (token->type == PIPE)
		{
			// Start new command
			current_cmd->next = create_new_command();
			current_cmd = current_cmd->next;
			pipeline->cmd_count++;
		}
		token = token->next;
	}
	return (pipeline);
}

// Helper functions for cleaner code

// void	print_pipeline(t_pipeline *pipeline)
// {
// 	int		cmd_num;
// 	t_cmd	*cmd;
// 	int		i;
// 	t_token	*redir;

// 	if (!pipeline)
// 	{
// 		printf("❌ Pipeline is NULL\n");
// 		return ;
// 	}
// 	printf("Pipeline with %d command(s):\n", pipeline->cmd_count);
// 	cmd_num = 1;
// 	cmd = pipeline->commands;
// 	while (cmd)
// 	{
// 		printf("  Command %d:\n", cmd_num++);
// 		printf("    Args: ");
// 		if (cmd->args)
// 		{
// 			i = 0;
// 			while (cmd->args[i])
// 			{
// 				printf("'%s' ", cmd->args[i]);
// 				i++;
// 			}
// 		}
// 		printf("\n");
// 		printf("    Redirections: ");
// 		redir = cmd->redirections;
// 		if (!redir)
// 			printf("(none)");
// 		while (redir)
// 		{
// 			printf("%d '%s' ", redir->type,
// 				redir->value ? redir->value : "NULL");
// 			redir = redir->next;
// 		}
// 		printf("\n");
// 		cmd = cmd->next;
// 	}
// 	printf("\n");
// }

// int	main(void)
// {
// 	char		input[256];
// 	size_t		len;
// 	t_token		*tokens;
// 	t_pipeline	*pipeline;

// 	printf("Enter command line: ");
// 	if (!fgets(input, sizeof(input), stdin))
// 	{
// 		printf("Input error!\n");
// 		return (1);
// 	}
// 	// Remove trailing newline
// 	len = strlen(input);
// 	if (len > 0 && input[len - 1] == '\n')
// 		input[len - 1] = '\0';
// 	tokens = tokenizer(input);
// 	if (!tokens)
// 	{
// 		printf("Tokenization failed!\n");
// 		return (1);
// 	}
// 	pipeline = parse(tokens);
// 	print_pipeline(pipeline);
// 	// Free tokens and pipeline if needed
// 	return (0);
// }
