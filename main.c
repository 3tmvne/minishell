/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 20:45:03 by ozemrani          #+#    #+#             */
/*   Updated: 2025/08/22 17:17:25 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_shell_state	*g_shell_state = NULL;

char	*heredoc(t_cmd *cmd)
{
	int		i;
	char	*file;
	t_token	*red;
	char	*heredoc_file;

	i = 0;
	file = NULL;
	red = cmd->redirections;
	while (red)
	{
		if (red->type == HEREDOC)
		{
			heredoc_file = handle_heredoc_file(red->value, i, red->quote);
			if (!heredoc_file) // heredoc was interrupted
				return (file); // Stop processing remaining heredocs
			// Additional check for global interruption state
			if (g_shell_state && g_shell_state->last_exit_status == 130)
				return (file);
			red->value = ft_strdup(heredoc_file);
		}
		if (red->next == NULL)
			break ;
		red = red->next;
		i++;
	}
	return (file);
}

void	setup_heredoc(t_cmd *cmd)
{
	t_cmd	*current_cmd;
	t_token	*red;

	current_cmd = cmd;
	while (current_cmd)
	{
		red = current_cmd->redirections;
		if (red)
		{
			heredoc(current_cmd);
			// If any heredoc was interrupted, stop processing remaining commands
			if (g_shell_state && g_shell_state->last_exit_status == 130)
				break;
		}
		current_cmd = current_cmd->next;
	}
}

void	executing(char *str, t_shell_state *shell)
{
	t_token		*tokens;
	t_pipeline	*cmds;

	if (g_shell_state && shell)
	{
		shell->last_exit_status = g_shell_state->last_exit_status;
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
	setup_heredoc(cmds->commands);
	// Check if heredoc setup was interrupted (Ctrl+C)
	if (g_shell_state && g_shell_state->last_exit_status == 130)
	{
		shell->last_exit_status = 130;
		return ;
	}
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
	g_shell_state = state;
	state->last_exit_status = 0;
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
		// Reset exit status if it was set by main shell Ctrl+C (not heredoc)
		if (state->last_exit_status == 130)
			state->last_exit_status = 0;
		str = readline("minishell$> ");
		if (!str)
			exit(state->last_exit_status);
		if (*str)
			add_history(str);
		add_to_gc(str);
		executing(str, state);
	}
}
