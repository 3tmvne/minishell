/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:45:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/24 17:58:12 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*init_env(char **env)
{
	t_env	*state_env;
	t_env	*cur;

	state_env = array_to_env_list(env);
	cur = state_env;
	while (cur)
	{
		add_flag_to_gc(cur);
		cur = cur->next;
	}
	return (state_env);
}

void	start_bash(char *str, t_shell_state *shell)
{
	t_token		*tokens;
	t_pipeline	*cmds;

	if (shell)
		shell->last_exit_status = get_shell_state(NULL)->last_exit_status;
	if (!str || !shell)
		return ;
	if (quote_syntax(str))
	{
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		shell->last_exit_status = 2;
		return ;
	}
	tokens = tokenizer(str);
	tokens = expand_tokens(tokens, shell);
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

	(void)ac;
	(void)av;
	state = ft_malloc(sizeof(t_shell_state));
	get_shell_state(state);
	state->env = init_env(env);
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
