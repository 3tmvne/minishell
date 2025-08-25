/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_tokens2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 22:11:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/25 22:16:33 by aregragu         ###   ########.fr       */
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
		return (NULL);
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
	t_token	*current;

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

int	check_redirection_after_pipe_helper(t_token *next_token)
{
	char	*error_token;

	if (next_token && (next_token->type == REDIR_OUT
			|| next_token->type == REDIR_IN || next_token->type == APPEND
			|| next_token->type == HEREDOC))
	{
		error_token = get_redirection_error_token(next_token->type);
		syntax_error(error_token);
		return (0);
	}
	return (1);
}
