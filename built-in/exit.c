/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 17:14:38 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/28 17:38:43 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>

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
		if ((sign == 1 && (num > LLONG_MAX / 10 || (num == LLONG_MAX / 10
						&& str[i] - '0' > LLONG_MAX % 10))) || (sign == -1
				&& (num > -(LLONG_MIN / 10) || (num == -(LLONG_MIN / 10)
						&& str[i] - '0' > -(LLONG_MIN % 10)))))
			return (0);
		num = num * 10 + (str[i++] - '0');
	}
	*result = num * sign;
	return (1);
}

static int	is_numeric_arg(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	print_exit_and_cleanup(int status)
{
	free_gc_all();
	exit(status);
}

static int	handle_exit_arguments(t_cmd *cmd, int last_status)
{
	int			arg_count;
	long long	status;

	arg_count = 0;
	while (cmd->args[arg_count + 1])
		arg_count++;
	if (arg_count == 0)
		print_exit_and_cleanup(last_status);
	if (!is_numeric_arg(cmd->args[1]) || !safe_atoll(cmd->args[1], &status))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		print_exit_and_cleanup(2);
	}
	if (arg_count > 1)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);
	}
	print_exit_and_cleanup((unsigned char)status);
	return (0);
}

int	exit_builtin(t_cmd *cmd, int last_status)
{
	ft_putstr_fd("exit\n", 2);
	if (!cmd)
		print_exit_and_cleanup(last_status);
	return (handle_exit_arguments(cmd, last_status));
}
