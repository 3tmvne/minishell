#include "minishell.h"

// Global pointer to shell state for exit status
extern t_shell_state *g_shell_state;

void handle_sigint(int sig)
{
    (void)sig;
    write(1, "\n", 1);  // Always write newline first
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
    if (g_shell_state)
        g_shell_state->last_exit_status = 130;
}

void handle_signals(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
}

// For child processes: restore default SIGINT
void set_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
