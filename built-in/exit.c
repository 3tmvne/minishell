#include "minishell.h"
#include <limits.h>

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
int	exit_builtin(t_cmd *cmd, int last_status)
{
	int			arg_count;
	long long	status;
	int			valid;

	ft_putstr_fd("exit\n", 1);
	
	if (!cmd)
		exit(last_status);
		
	// Count arguments (excluding "exit" command itself)
	arg_count = 0;
	while (cmd->args[arg_count + 1])
		arg_count++;
	
	if (arg_count == 0)
		exit(last_status);
	
	if (!is_numeric_arg(cmd->args[1]))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		exit(2);  // Exit with status 2 for non-numeric arguments (bash behavior)
	}
	
	valid = safe_atoll(cmd->args[1], &status);
	if (!valid)
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
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
