# Minishell Parsing Explained

## Overview

Parsing in minishell is the process of converting a raw command line (like `ls -la | grep txt > output.txt`) into structured data that your shell can execute. This involves breaking the input into tokens, grouping tokens into commands, handling pipes and redirections, and building a pipeline structure.

---

## Steps in Parsing

### 1. **Tokenization**
- The input string is split into tokens.
- Tokens represent words, operators (`|`, `>`, `<`, etc.), and filenames.
- Example tokens for `ls -la | grep txt > output.txt`:
  - `ls` (WORD)
  - `-la` (WORD)
  - `|` (PIPE)
  - `grep` (WORD)
  - `txt` (WORD)
  - `>` (REDIR_OUT)
  - `output.txt` (WORD)

### 2. **Building Commands**
- Tokens are grouped into commands.
- Each command consists of arguments and possible redirections.
- Commands are separated by pipes (`|`).

### 3. **Handling Redirections**
- Redirection tokens (`>`, `<`, `>>`, etc.) are attached to the current command.
- The filename following a redirection operator is stored as part of the redirection.

### 4. **Constructing the Pipeline**
- Commands are linked together in a pipeline structure.
- Each command is a node in a linked list.
- The pipeline keeps track of the number of commands.

---

## Data Structures

```c
typedef struct s_cmd {
    char **args;            // Command arguments
    t_token *redirections;  // List of redirection tokens
    struct s_cmd *next;     // Next command in pipeline
} t_cmd;

typedef struct s_pipeline {
    t_cmd *commands;        // Linked list of commands
    int cmd_count;          // Number of commands
} t_pipeline;
```

---

## Example

**Input:**  
`ls -la | grep txt > output.txt`

**Parsed Structure:**

- `t_pipeline`
  - `cmd_count = 2`
  - `commands =`
    - `t_cmd 1: args = ["ls", "-la", NULL], redirections = NULL`
    - `t_cmd 2: args = ["grep", "txt", NULL], redirections = [> output.txt]`

---

## Parsing Function

The parser walks through the token list:
- Collects arguments until a pipe or redirection is found.
- On a pipe, starts a new command.
- On a redirection, attaches it to the current command.
- Builds the pipeline as a linked list of commands.

---

## Summary

Parsing in minishell transforms a command line into a structured pipeline:
- **Tokenize** the input.
- **Group tokens** into commands and redirections.
- **Build a pipeline** for execution.

This structure makes it easy for your shell to execute commands, handle pipes, and