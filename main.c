/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:45:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/23 01:37:23 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	heredoc(t_cmd *cmd)
{
	int		i;
	t_token	*red;
	char	*heredoc_file;

	i = 0;
	red = cmd->redirections;
	while (red)
	{
		if (red->type == HEREDOC)
		{
			heredoc_file = handle_heredoc_file(red->value, i, red->quote);
			if (!heredoc_file)
				return (1);
			if (get_shell_state(NULL)->last_exit_status == 130)
				return (1);
			red->value = ft_strdup(heredoc_file);
		}
		red = red->next;
		i++;
	}
	return (0);
}

int	setup_heredoc(t_cmd *cmd)
{
	t_cmd	*current_cmd;
	t_token	*red;

	current_cmd = cmd;
	while (current_cmd)
	{
		red = current_cmd->redirections;
		if (red)
		{
			if (heredoc(current_cmd))
				return (1);
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}

void	start_bash(char *str, t_shell_state *shell)
{
	t_token		*tokens;
	t_pipeline	*cmds;

	if (shell)
	{
		shell->last_exit_status = get_shell_state(NULL)->last_exit_status;
	}
	if (!str || !shell)
		return ;
	if (quote_syntax(str))
	{
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		shell->last_exit_status = 2;
		return ;
	}
	tokens = tokenizer(str);
	tokens = expand_tokens_selective(tokens, shell);
	cmds = parse(tokens);
	if (setup_heredoc(cmds->commands))
		return ;
	if (check_syntax(tokens))
	{
		shell->last_exit_status = 2;
		return ;
	}
	execute(cmds, shell);
}

int	main(int ac, char **av, char **env)
{
	char			*str;
	t_shell_state	*state;
	t_env			*cur;

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	get_shell_state(state);
	state->env = array_to_env_list(env);
	cur = state->env;
	while (cur)
	{
		add_flag_to_gc(cur);
		cur = cur->next;
	}
	while (1)
	{
		handle_signals();
		str = readline("minishell$> ");
		if (!str)
			exit(state->last_exit_status);
		if (*str)
			add_history(str);
		else
			continue ;
		add_to_gc(str);
		start_bash(str, state);
	}
}
