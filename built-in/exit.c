#include "minishell.h"
#include <limits.h>

/**
 * Counts the number of arguments in args array
 */
static int	count_token_args(t_token *tokens)
{
	int		count;
	t_token	*current;

	count = 0;
	current = tokens->next; // Skip command name
	
	while (current && current->type == WORD)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * Converts a string to a long long integer with overflow detection
 * Returns 1 if successful, 0 if overflow occurred
 */
static int	safe_atoll(const char *str, long long *result)
{
	long long	num;
	int			sign;
	int			i;

	num = 0;
	sign = 1;
	i = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		// Check for overflow before adding the next digit
		if ((sign == 1 && (num > LLONG_MAX / 10 || 
			(num == LLONG_MAX / 10 && str[i] - '0' > LLONG_MAX % 10))) ||
			(sign == -1 && (num > -(LLONG_MIN / 10) || 
			(num == -(LLONG_MIN / 10) && str[i] - '0' > -(LLONG_MIN % 10)))))
			return (0);
		num = num * 10 + (str[i++] - '0');
	}
	*result = num * sign;
	return (1);
}
/**
 * Checks if a string is a valid numeric argument
 * Allows for a leading '+' or '-' sign
 */
static int	is_numeric_arg(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])  // Just a sign with no digits
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Implements the exit builtin command
 * Exits the shell with the specified status
 */
int	exit_builtin(t_token *tokens, t_shell_state *shell)
{
	int			arg_count;
	long long	status;
	int			valid;
	t_token		*arg_token;

	ft_putstr_fd("exit\n", 1);
	
	if (!tokens)
		exit(shell->last_exit_status);
		
	arg_count = count_token_args(tokens);
	arg_token = tokens->next; // Premier argument après "exit"
	
	if (arg_count == 0)
		exit(shell->last_exit_status);
	
	if (!is_numeric_arg(arg_token->value))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg_token->value, 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		exit(2);  // Exit with status 2 for non-numeric arguments (bash behavior)
	}
	
	valid = safe_atoll(arg_token->value, &status);
	if (!valid)
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg_token->value, 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		exit(2);  // Exit with status 2 for overflow (bash behavior)
	}
	
	if (arg_count > 1)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);  // Don't exit, return error status
	}
	
	// Convert to 8-bit integer by taking modulo 256
	exit((unsigned char)status);  // This ensures the status is 0-255
}
