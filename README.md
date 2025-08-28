# Minishell

A simple shell implementation written in C, recreating basic functionality of bash.

## Description

Minishell is a project that involves creating a simple shell program that can execute commands, handle redirections, pipes, and environment variables. This project helps understand how shells work under the hood and provides hands-on experience with system calls, process management, and parsing.

## Features

- Command execution with arguments
- Built-in commands (`echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`)
- Input/Output redirections (`<`, `>`, `>>`)
- Pipes (`|`)
- Environment variable expansion (`$VAR`)
- Signal handling (`Ctrl+C`, `Ctrl+D`, `Ctrl+\`)
- Command history
- Quote handling (single and double quotes)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/3tmvne/minishell.git
cd minishell
```

2. Compile the project:
```bash
make
```

3. Run the shell:
```bash
./minishell
```

## Usage

Once you start minishell, you can use it like a regular shell:

```bash
$ echo "Hello, World!"
Hello, World!

$ ls -la | grep minishell
drwxr-xr-x  3 user user 4096 Aug 28 17:49 minishell

$ export MY_VAR="test"
$ echo $MY_VAR
test

$ cd /tmp
$ pwd
/tmp
```

### Built-in Commands

- `echo [options] [string]` - Display text
- `cd [directory]` - Change current directory
- `pwd` - Print working directory
- `export [var=value]` - Set environment variable
- `unset [var]` - Unset environment variable
- `env` - Display environment variables
- `exit [status]` - Exit the shell

### Redirections

- `command > file` - Redirect output to file
- `command >> file` - Append output to file
- `command < file` - Redirect input from file

### Pipes

- `command1 | command2` - Pipe output of command1 to input of command2

## Project Structure

```
minishell/
├── src/           # Source files
├── include/       # Header files
├── Makefile       # Build configuration
└── README.md      # This file
```

## Compilation

The project uses a Makefile with the following targets:

- `make` or `make all` - Compile the project
- `make clean` - Remove object files
- `make fclean` - Remove object files and executable
- `make re` - Recompile the project

## Requirements

- GCC compiler
- Make
- POSIX-compliant system (Linux/macOS)
- GNU Readline library (for command history and line editing)

## Implementation Details

This shell implementation handles:

- **Lexical Analysis**: Tokenizing input into commands, arguments, and operators
- **Parsing**: Building command structures and handling syntax
- **Execution**: Forking processes and executing commands
- **Built-ins**: Implementing shell built-in commands
- **Redirections**: Managing file descriptors for I/O redirection
- **Pipes**: Creating and managing pipes between processes
- **Environment**: Handling environment variables and expansion
- **Signals**: Proper signal handling for interactive use

## Error Handling

The shell handles various error conditions:
- Invalid commands
- Permission errors
- File not found errors
- Syntax errors in command input
- Signal interruptions

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Create a Pull Request

## License

This project is part of a programming curriculum and is intended for educational purposes.

## Author

**3tmvne** - [GitHub Profile](https://github.com/3tmvne)

---

*This project is inspired by the bash shell and implements a subset of its functionality for educational purposes.*
