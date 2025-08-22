#include "minishell.h"

static int	is_valid_varname(const char *name)
{
	int	i;

	if (!name || !*name || name[0] == '=' || ft_isdigit(name[0]))
		return (0);
	if (name[0] != '_' && !ft_isalpha(name[0]))
		return (0);
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (name[i] != '_' && !ft_isalnum(name[i]))
			return (0);
		i++;
	}
	return (1);
}

static char	**create_sorted_env_for_export(t_env *env)
{
	int		count;
	t_env	*current;
	char	**array;
	int		i;
	int		name_len;
	int		value_len;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	array = ft_malloc(sizeof(char *) * (count + 1));
	current = env;
	i = 0;
	while (current)
	{
		name_len = ft_strlen(current->name);
		if (current->value)
		{
			value_len = ft_strlen(current->value);
			array[i] = ft_malloc(name_len + value_len + 2);
			ft_strlcpy(array[i], current->name, name_len + 1);
			ft_strlcat(array[i], "=", name_len + 2);
			ft_strlcat(array[i], current->value, name_len + value_len + 2);
		}
		else
		{
			array[i] = ft_malloc(name_len + 1);
			ft_strlcpy(array[i], current->name, name_len + 1);
		}
		current = current->next;
		i++;
	}
	array[i] = NULL;
	return (array);
}

static void	print_exported_env(t_env *env)
{
	char	**sorted_env;
	int		i;
	char	*eq;
	int		name_len;

	sorted_env = create_sorted_env_for_export(env);
	if (!sorted_env)
		return ;
	i = 0;
	while (sorted_env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		eq = ft_strchr(sorted_env[i], '=');
		if (eq)
		{
			name_len = eq - sorted_env[i];
			write(STDOUT_FILENO, sorted_env[i], name_len);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(eq + 1, STDOUT_FILENO);
			ft_putstr_fd("\"\n", STDOUT_FILENO);
		}
		else
		{
			ft_putstr_fd(sorted_env[i], STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		i++;
	}
}

static char	*extract_var_name(const char *var)
{
	int	i;

	i = 0;
	while (var[i] && var[i] != '=')
		i++;
	return (ft_substr(var, 0, i));
}

static int	handle_export_var(const char *var, t_env **env)
{
	char	*name;
	char	*equals_pos;
	char	*value;
	char	*parsed_value;
	int		len;
	t_env	*existing;
	t_env	*new_var;
	t_env	*cur;

	name = extract_var_name(var);
	if (!name || !is_valid_varname(name))
	{
		ft_putstr_fd("export: '", STDERR_FILENO);
		ft_putstr_fd((char *)var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (1);
	}
	equals_pos = ft_strchr(var, '=');
	if (equals_pos)
	{
		value = equals_pos + 1;
		parsed_value = ft_strdup(value);
		len = ft_strlen(parsed_value);
		while (len > 0 && (parsed_value[len - 1] == '\n' || parsed_value[len
				- 1] == '\r'))
		{
			parsed_value[len - 1] = '\0';
			len--;
		}
		set_env_var(env, name, parsed_value);
		return (0);
	}
	else
	{
		existing = find_env_var(*env, name);
		if (!existing)
		{
			new_var = create_env_node(name, NULL);
			if (new_var)
			{
				cur = *env;
				if (!cur)
					*env = new_var;
				else
				{
					while (cur->next)
						cur = cur->next;
					cur->next = new_var;
				}
			}
		}
		return (0);
	}
}

int	export_builtin(t_cmd *cmd, t_env **env)
{
	int	i;
	int	exit_status;
	int	result;

	exit_status = 0;
	if (!cmd->args[1])
	{
		print_exported_env(*env);
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (!cmd->args[i] || cmd->args[i][0] == '\0')
		{
			i++;
			continue ;
		}
		result = handle_export_var(cmd->args[i], env);
		if (result != 0)
			exit_status = result;
		i++;
	}
	return (exit_status);
}

char	**get_filtered_env_list(t_env *env)
{
	int		count;
	t_env	*current;
	char	**filtered_env;
	int		i;
	int		name_len;
	int		value_len;

	count = 0;
	current = env;
	while (current)
	{
		if (current->value)
			count++;
		current = current->next;
	}
	filtered_env = ft_malloc(sizeof(char *) * (count + 1));
	current = env;
	i = 0;
	while (current)
	{
		if (current->value)
		{
			name_len = ft_strlen(current->name);
			value_len = ft_strlen(current->value);
			filtered_env[i] = ft_malloc(name_len + value_len + 2);
			if (filtered_env[i])
			{
				ft_strlcpy(filtered_env[i], current->name, name_len + 1);
				ft_strlcat(filtered_env[i], "=", name_len + 2);
				ft_strlcat(filtered_env[i], current->value, name_len + value_len
					+ 2);
				i++;
			}
		}
		current = current->next;
	}
	filtered_env[i] = NULL;
	return (filtered_env);
}
