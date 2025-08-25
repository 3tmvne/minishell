/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_signals.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:43:57 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/25 19:34:11 by aregragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_sigint(int sig)
{
	t_shell_state	*state;

	(void)sig;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	state = get_shell_state(NULL);
	if (state)
		state->last_exit_status = 130;
}

t_shell_state	*get_shell_state(t_shell_state *shell_state)
{
	static t_shell_state	*save = NULL;

	if (shell_state)
		save = shell_state;
	return (save);
}

void	handler_signal_heredoc(int sig)
{
	t_shell_state	*state;

	(void)sig;
	write(1, "\n", 1);
	state = get_shell_state(NULL);
	if (state)
		state->last_exit_status = 130;
	free_gc_all();
	exit(130);
}

void	handle_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
