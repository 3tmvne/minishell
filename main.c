/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:45:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 22:23:19 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	state->last_exit_status = 0;
	state->env = array_to_env_list(env);
	while (1)
	{
		handle_signals();
		str = readline("minishell$ ");
		add_to_gc(str);
		if (!str)
			exit_builtin(NULL, state->last_exit_status);
		if (*str)
			add_history(str);
		else
			continue ;
		start_bash(str, state);
	}
}
