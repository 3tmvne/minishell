#include "minishell.h"

t_cmd	*create_new_command(void)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
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

	if (!arg)
		return;
	
	count = 0;
	if (cmd->args)
	{
		while (cmd->args[count])
			count++;
	}
	new_args = ft_malloc(sizeof(char *) * (count + 2));
	if (cmd->args)
	{
		i = 0;
		while (i < count)
		{
			new_args[i] = cmd->args[i];
			i++;
		}
	}
	new_args[count] = arg;
	new_args[count + 1] = NULL;
	cmd->args = new_args;
}

void	add_redirection(t_cmd *cmd, t_token_type type, char *filename, t_quote_type quote_type)
{
	t_token	*redir;
	t_token	*last;

	redir = ft_calloc(1, sizeof(t_token));
	redir->value = filename;
	redir->type = type;
	redir->quote = quote_type; // Préserver le type de guillemets
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

	pipeline = ft_calloc(1, sizeof(t_pipeline));
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
		else if (token->type == REDIR_OUT || token->type == REDIR_IN
			|| token->type == APPEND || token->type == HEREDOC)
		{
			// Skip any WS tokens after redirection
			next = token->next;
			while (next && next->type == WS)
				next = next->next;
			// Only add if next is a WORD (filename)
			if (next && next->type == WORD)
			{
				char *filename = next->value;
				// t_quote_type filename_quote_type = next->quote;
				
				// Pour les heredocs, concaténer tous les tokens WORD consécutifs
				if (token->type == HEREDOC)
				{
					t_token *concat_token = next->next;
					while (concat_token && concat_token->type == WORD)
					{
						char *old_filename = filename;
						filename = ft_strjoin(filename, concat_token->value);
						if (old_filename != next->value)
							free(old_filename);
						next = concat_token;
						concat_token = concat_token->next;
					}
				}
				
				add_redirection(current_cmd, token->type, filename, next->quote);
				token = next; // Skip the filename token(s)
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
