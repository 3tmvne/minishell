/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:15:46 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 01:42:07 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_redirection_error_token(int type)
{
	if (type == REDIR_OUT)
		return ("'>'");
	else if (type == REDIR_IN)
		return ("'<'");
	else if (type == APPEND)
		return ("'>>'");
	else if (type == HEREDOC)
		return ("'<<'");
	else if (type == PIPE)
		return ("'|'");
	else
		return ("redirection");
}

int	check_invalid_after_redirection(t_token *redir_token)
{
	char	*error_token;

	if (redir_token->next->type == REDIR_OUT
		|| redir_token->next->type == REDIR_IN
		|| redir_token->next->type == APPEND
		|| redir_token->next->type == HEREDOC
		|| redir_token->next->type == PIPE)
	{
		error_token = get_redirection_error_token(redir_token->next->type);
		syntax_error(error_token);
		return (0);
	}
	return (1);
}

int	is_redirection_valid(t_token *redir_token)
{
	if (!redir_token->next)
		return (0);
	if (!check_invalid_after_redirection(redir_token))
		return (0);
	if (redir_token->next->type == WORD)
		return (1);
	if (redir_token->next->type == WS && redir_token->next->next
		&& redir_token->next->next->type == WORD)
		return (1);
	if (redir_token->prev && redir_token->prev->type == PIPE)
		return (1);
	return (0);
}

int	is_valid_after_pipe(t_token *token)
{
	t_token *current;
	
	if (!token)
		return (0);
	current = token;
	while (current && current->type == WS)
		current = current->next;
	if (!current)
		return (0);
	if (current->type == WORD || current->type == REDIR_OUT
		|| current->type == REDIR_IN || current->type == APPEND
		|| current->type == HEREDOC)
		return (1);
	return (0);
}

t_token	*skip_whitespace(t_token *token)
{
	while (token && token->type == WS)
		token = token->next;
	return (token);
}

int	check_redirection_after_pipe_helper(t_token *next_token)
{
	char	*error_token;

	if (next_token && (next_token->type == REDIR_OUT
		|| next_token->type == REDIR_IN
		|| next_token->type == APPEND
		|| next_token->type == HEREDOC))
	{
		error_token = get_redirection_error_token(next_token->type);
		syntax_error(error_token);
		return (0);
	}
	return (1);
}

int	check_redirection_after_pipe(t_token *redir_token)
{
	t_token	*next_token;

	next_token = skip_whitespace(redir_token->next);
	if (next_token && next_token->type == PIPE)
	{
		next_token = skip_whitespace(next_token->next);
		if (!check_redirection_after_pipe_helper(next_token))
			return (0);
	}
	return (1);
}

int	check_pipe_edges(t_token *current)
{
	if (!current->prev || (current->prev && current->prev->type == WS
			&& !current->prev->prev))
	{
		syntax_error("'|'");
		return (2);
	}
	if (!current->next)
	{
		syntax_error("newline");
		return (2);
	}
	if (current->next && current->next->type == WS && !current->next->next)
	{
		syntax_error("newline");
		return (2);
	}
	return (0);
}

static int	check_double_pipes(t_token *current)
{
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
	return (0);
}

int	check_pipe_syntax(t_token *tokens)
{
	t_token	*current;
	int		result;

	current = tokens;
	while (current)
	{
		if (current->type == PIPE)
		{
			result = check_pipe_edges(current);
			if (result != 0)
				return (result);
			result = check_double_pipes(current);
			if (result != 0)
				return (result);
		}
		current = current->next;
	}
	return (0);
}

static int	check_redirection_edge_cases(t_token *current)
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
	return (0);
}

int	check_redirection_syntax(t_token *tokens)
{
	t_token	*current;
	int		result;

	current = tokens;
	while (current)
	{
		if (current->type == REDIR_OUT || current->type == REDIR_IN
			|| current->type == APPEND || current->type == HEREDOC)
		{
			result = check_redirection_edge_cases(current);
			if (result != 0)
				return (result);
			if (!check_redirection_after_pipe(current))
				return (2);
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