/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:15:46 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/21 23:47:08 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirection_valid(t_token *redir_token)
{
	char	*error_token;

	if (!redir_token->next)
		return (0);
	if (redir_token->next->type == REDIR_OUT
		|| redir_token->next->type == REDIR_IN
		|| redir_token->next->type == APPEND
		|| redir_token->next->type == HEREDOC
		|| redir_token->next->type == PIPE)
	{
		if (redir_token->next->type == REDIR_OUT)
			error_token = "'>'";
		else if (redir_token->next->type == REDIR_IN)
			error_token = "'<'";
		else if (redir_token->next->type == APPEND)
			error_token = "'>>'";
		else if (redir_token->next->type == HEREDOC)
			error_token = "'<<'";
		else if (redir_token->next->type == PIPE)
			error_token = "'|'";
		else
			error_token = "redirection";
		syntax_error(error_token);
		return (0);
	}
	if (redir_token->next->type == WORD)
		return (1);
	if (redir_token->next->type == WS && redir_token->next->next
		&& redir_token->next->next->type == WORD)
		return (1);
	if (redir_token->prev && redir_token->prev->type == PIPE)
		return (1);
	return (0);
}

static int	is_valid_after_pipe(t_token *token)
{
	if (!token)
		return (0);
	if (token->type == WORD || token->type == REDIR_OUT
		|| token->type == REDIR_IN || token->type == APPEND
		|| token->type == HEREDOC)
		return (1);
	if (token->type == WS && token->next && (token->next->type == WORD
			|| token->next->type == REDIR_OUT || token->next->type == REDIR_IN
			|| token->next->type == APPEND || token->next->type == HEREDOC))
		return (1);
	return (0);
}

int	check_pipe_syntax(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == PIPE)
		{
			if (!current->prev || (current->prev && current->prev->type == WS
					&& !current->prev->prev))
			{
				syntax_error("'|'");
				return (2);
			}
			if (!current->next || (current->next && current->next->type == WS
					&& !current->next->next))
			{
				syntax_error("newline");
				return (2);
			}
			if (current->next && current->next->type == PIPE)
			{
				syntax_error("'|'");
				return (2);
			}
			if (current->next && current->next->type == WS
				&& current->next->next && current->next->next->type == PIPE)
			{
				syntax_error("'|'");
				return (2);
			}
			if (!is_valid_after_pipe(current->next))
			{
				syntax_error("unexpected token after pipe");
				return (2);
			}
		}
		current = current->next;
	}
	return (0);
}

int	check_redirection_syntax(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == REDIR_OUT || current->type == REDIR_IN
			|| current->type == APPEND || current->type == HEREDOC)
		{
			if (!current->next || (current->next && current->next->type == WS
					&& !current->next->next))
			{
				syntax_error("newline");
				return (2);
			}
			if (current->next && current->next->type == REDIR_OUT)
			{
				syntax_error("'>'");
				return (2);
			}
			if (current->next && current->next->type == REDIR_IN)
			{
				syntax_error("'<'");
				return (2);
			}
			if (!is_redirection_valid(current))
				return (2);
		}
		current = current->next;
	}
	return (0);
}

int	check_syntax(t_token *tokens)
{
	int	pipe_result;
	int	redir_result;

	if (!tokens)
		return (0);
	pipe_result = check_pipe_syntax(tokens);
	if (pipe_result != 0)
		return (1);
	redir_result = check_redirection_syntax(tokens);
	if (redir_result != 0)
		return (1);
	return (0);
}
