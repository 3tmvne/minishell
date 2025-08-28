/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:01:24 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/26 20:29:36 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*get_spaces(char *input, int *i)
{
	char	*space;
	t_token	*new_token;

	space = ft_strdup(" ");
	new_token = create_token(space, WS, NQUOTES);
	while (input[*i] && is_space(input[*i]))
	{
		(*i)++;
	}
	return (new_token);
}

t_token	*get_word(char *input, int *i)
{
	t_token	*new_token;
	char	*word;
	int		idx;

	idx = ft_strcspn(&input[*i], " \t\"'|><");
	word = ft_substr(input, *i, idx);
	*i += idx;
	new_token = create_token(word, WORD, NQUOTES);
	return (new_token);
}

t_token	*get_quoted(char *input, int *i)
{
	t_token			*new_token;
	char			*quote;
	t_quote_type	quote_type;
	char			quote_char;

	int (start), (quoted_length);
	if (input[*i] == '"')
		quote_type = DQUOTES;
	else if (input[*i] == '\'')
		quote_type = SQUOTES;
	start = *i;
	quote_char = input[start];
	(*i)++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] == quote_char)
		(*i)++;
	quoted_length = *i - start - 2;
	if (quoted_length <= 0)
		quote = ft_strdup("");
	else
		quote = ft_substr(input, start + 1, quoted_length);
	new_token = create_token(quote, WORD, quote_type);
	return (new_token);
}

t_token_type	get_token_type(char *word)
{
	if (ft_strcmp(word, "|") == 0)
		return (PIPE);
	else if (ft_strcmp(word, ">") == 0)
		return (REDIR_OUT);
	else if (ft_strcmp(word, ">>") == 0)
		return (APPEND);
	else if (ft_strcmp(word, "<") == 0)
		return (REDIR_IN);
	else if (ft_strcmp(word, "<<") == 0)
		return (HEREDOC);
	else
		return (WORD);
}

t_token	*get_op(char *input, int *i)
{
	t_token	*new_token;

	new_token = NULL;
	if (input[*i] == '|')
	{
		new_token = create_token(char_to_str(input[*i]), PIPE, NQUOTES);
		(*i)++;
	}
	else if (input[*i] == '<' || input[*i] == '>')
	{
		if (input[*i + 1] && input[*i + 1] == input[*i])
		{
			new_token = create_token(ft_substr(input, *i, 2),
					get_token_type(ft_substr(input, *i, 2)), NQUOTES);
			(*i) += 2;
		}
		else
		{
			new_token = create_token(char_to_str(input[*i]),
					get_token_type(char_to_str(input[*i])), NQUOTES);
			(*i)++;
		}
	}
	return (new_token);
}
