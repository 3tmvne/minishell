/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:07:33 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/23 22:07:43 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static char	*build_heredoc_filename(t_token *next)
{
	t_token	*concat_token;
	char	*filename;

	filename = next->value;
	concat_token = next->next;
	while (concat_token && concat_token->type == WORD)
	{
		filename = ft_strjoin(filename, concat_token->value);
		concat_token = concat_token->next;
	}
	return (filename);
}

static t_token	*handle_redirection(t_cmd *current_cmd, t_token *token)
{
	t_token	*next;
	char	*filename;

	next = token->next;
	while (next && next->type == WS)
		next = next->next;
	if (next && next->type == WORD)
	{
		if (token->type == HEREDOC)
			filename = build_heredoc_filename(next);
		else
			filename = next->value;
		add_redirection(current_cmd, token->type, filename, next->quote);
		if (token->type == HEREDOC)
		{
			while (next->next && next->next->type == WORD)
				next = next->next;
		}
		return (next);
	}
	return (token);
}

static t_cmd	*handle_pipe(t_cmd *current_cmd, t_pipeline *pipeline)
{
	current_cmd->next = create_new_command();
	current_cmd = current_cmd->next;
	pipeline->cmd_count++;
	return (current_cmd);
}

t_pipeline	*parse(t_token *tokens)
{
	t_pipeline	*pipeline;
	t_cmd		*current_cmd;
	t_token		*token;

	pipeline = ft_calloc(1, sizeof(t_pipeline));
	current_cmd = create_new_command();
	pipeline->commands = current_cmd;
	pipeline->cmd_count = 1;
	token = tokens;
	while (token)
	{
		if (token->type == WORD)
			add_argument(current_cmd, token->value);
		else if (token->type == REDIR_OUT || token->type == REDIR_IN
			|| token->type == APPEND || token->type == HEREDOC)
			token = handle_redirection(current_cmd, token);
		else if (token->type == PIPE)
			current_cmd = handle_pipe(current_cmd, pipeline);
		token = token->next;
	}
	return (pipeline);
}
