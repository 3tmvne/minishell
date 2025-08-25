/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 21:15:46 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:15:47 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static int	check_double_pipes(t_token *current)
{
	if (current->next && current->next->type == PIPE)
	{
		syntax_error("'|'");
		return (2);
	}
	if (current->next && current->next->type == WS && current->next->next
		&& current->next->next->type == PIPE)
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
