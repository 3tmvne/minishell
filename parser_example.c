#include "minishell.h"

// Structure for a single command
typedef struct s_cmd
{
    char            **args;         // Command arguments: ["ls", "-la", NULL]
    t_token         *redirections;  // !List of redirection tokens
    struct s_cmd    *next;         // Next command in pipeline
}   t_cmd;

// Structure for the entire command line
typedef struct s_pipeline
{
    t_cmd           *commands;      // Linked list of commands
    int             cmd_count;      // Number of commands in pipeline
}   t_pipeline;

/* Example of what your parser should create:
* Input: "ls -la | grep txt > output.txt"
* Result:
* t_pipeline {
*     cmd_count = 2,
*     commands = {
*         t_cmd {
*             args = ["ls", "-la", NULL],
*             redirections = NULL,
*             next = &cmd2
*         },
*         t_cmd {
*             args = ["grep", "txt", NULL], 
*             redirections = [> output.txt],
*             next = NULL
*         }
*     }
*/

/** parse_tokens - Parses a linked list of tokens into a pipeline structure.
 * @tokens: The head of the token linked list.
 *
 * ? Returns a pointer to the parsed pipeline structure, or NULL on failure.
 */
