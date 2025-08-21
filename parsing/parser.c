/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:07:33 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 21:07:51 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		return ;
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

void	add_redirection(t_cmd *cmd, t_token_type type, char *filename,
		t_quote_type quote_type)
{
	t_token	*redir;
	t_token	*last;

	redir = ft_calloc(1, sizeof(t_token));
	redir->value = filename;
	redir->type = type;
	redir->quote = quote_type;
	redir->next = NULL;
	redir->prev = NULL;
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
	t_token		*concat_token;
	char		*filename;
	char		*old_filename;

	pipeline = ft_calloc(1, sizeof(t_pipeline));
	current_cmd = NULL;
	token = tokens;
	current_cmd = create_new_command();
	pipeline->commands = current_cmd;
	pipeline->cmd_count = 1;
	while (token)
	{
		if (token->type == WORD)
			add_argument(current_cmd, token->value);
		else if (token->type == REDIR_OUT || token->type == REDIR_IN
			|| token->type == APPEND || token->type == HEREDOC)
		{
			next = token->next;
			while (next && next->type == WS)
				next = next->next;
			if (next && next->type == WORD)
			{
				filename = next->value;
				if (token->type == HEREDOC)
				{
					concat_token = next->next;
					while (concat_token && concat_token->type == WORD)
					{
						old_filename = filename;
						filename = ft_strjoin(filename, concat_token->value);
						if (old_filename != next->value)
							free(old_filename);
						next = concat_token;
						concat_token = concat_token->next;
					}
				}
				add_redirection(current_cmd, token->type, filename,
					next->quote);
				token = next;
			}
		}
		else if (token->type == PIPE)
		{
			current_cmd->next = create_new_command();
			current_cmd = current_cmd->next;
			pipeline->cmd_count++;
		}
		token = token->next;
	}
	return (pipeline);
}
