#include "minishell.h"

// Assumed token types
typedef enum e_token_type { WORD, PIPE, REDIR_OUT, REDIR_IN, REDIR_APPEND } t_token_type;

typedef struct s_token {
    t_token_type    type;
    char            *value;
    struct s_token  *next;
} t_token;

typedef struct s_cmd {
    char            **args;
    t_token         *redirections;
    struct s_cmd    *next;
} t_cmd;

typedef struct s_pipeline {
    t_cmd           *commands;
    int             cmd_count;
} t_pipeline;

// Helper to add argument to array
static void add_arg(char ***args, int *argc, char *value) {
    (*args) = realloc(*args, sizeof(char*) * (*argc + 2));
    (*args)[*argc] = value;
    (*argc)++;
    (*args)[*argc] = NULL;
}

// Helper to add redirection to linked list
static void add_redir(t_token **redirs, t_token *redir) {
    if (!*redirs) {
        *redirs = redir;
    } else {
        t_token *tmp = *redirs;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = redir;
    }
}

// Main parser function
t_pipeline *parse_tokens(t_token *tokens)
{
    t_pipeline *pipeline = calloc(1, sizeof(t_pipeline));
    t_cmd *last_cmd = NULL;
    t_token *t = tokens;

    while (t) {
        t_cmd *cmd = calloc(1, sizeof(t_cmd));
        char **args = NULL;
        int argc = 0;
        t_token *redirs = NULL;

        // Collect args and redirections until PIPE or end
        while (t && t->type != PIPE) {
            if (t->type == WORD) {
                add_arg(&args, &argc, t->value);
                t = t->next;
            } else if (t->type == REDIR_OUT || t->type == REDIR_IN || t->type == REDIR_APPEND) {
                t_token *redir = calloc(1, sizeof(t_token));
                redir->type = t->type;
                redir->value = t->value;
                redir->next = calloc(1, sizeof(t_token));
                redir->next->type = WORD;
                redir->next->value = t->next ? t->next->value : NULL;
                redir->next->next = NULL;
                add_redir(&redirs, redir);
                t = t->next ? t->next->next : NULL;
            } else {
                t = t->next;
            }
        }
        cmd->args = args;
        cmd->redirections = redirs;
        cmd->next = NULL;

        if (!pipeline->commands)
            pipeline->commands = cmd;
        else
            last_cmd->next = cmd;
        last_cmd = cmd;
        pipeline->cmd_count++;

        if (t && t->type == PIPE)
            t = t->next;
    }
    return pipeline;
}