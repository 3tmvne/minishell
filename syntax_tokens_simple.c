#include "minishell.h"

int	is_redirection_valid(t_token *redir_token) //>  j
{
	if (!redir_token->next)
		return (0);

	if (redir_token->next->type == WORD)
		return (1);

	if (redir_token->next->type == WS && redir_token->next->next
		&& redir_token->next->next->type == WORD)
		return (1);
	if (redir_token->prev->type == PIPE)
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
				syntax_error("|");
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
				syntax_error("|");
				return (2);
			}
			if (current->next && current->next->type == WS
				&& current->next->next && current->next->next->type == PIPE)
			{
				syntax_error("|");
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
			if (!is_redirection_valid(current))
			{
				if (!current->next)
					syntax_error("newline");
				else
					syntax_error("unexpected token after redirection");
				return (2);
			}
		}
		current = current->next;
	}
	return (0);
}
