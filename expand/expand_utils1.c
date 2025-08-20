/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/20 15:30:42 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Use libft's ft_strcmp implementation; local definition removed. */

void	ensure_capacity(t_parser_state *ps, size_t needed)
{
	size_t	new_cap;

	if (ps->out_pos + needed >= ps->out_capacity)
	{
		new_cap = (ps->out_capacity + needed) * 2;
		ps->output = realloc(ps->output, new_cap);
		ps->out_capacity = new_cap;
	}
}

void	append_output(t_parser_state *ps, const char *str, char c)
{
	size_t	len;

	if (str)
	{
		len = ft_strlen(str);
		ensure_capacity(ps, len);
		ft_memcpy(ps->output + ps->out_pos, str, len);
		ps->out_pos += len;
	}
	else if (c)
	{
		ensure_capacity(ps, 1);
		ps->output[ps->out_pos++] = c;
	}
}

char	*collapse_whitespace(const char *str)
{
	char	*result;
	int		i, j, in_space;

	if (!str)
		return (ft_strdup(""));
	
	/* Allouer la mémoire pour le résultat */
	result = ft_malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	
	/* Ignorer les espaces au début */
	i = 0;
	while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
		i++;
	
	/* Normaliser les espaces */
	j = 0;
	in_space = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			in_space = 1;
		}
		else
		{
			/* Ajouter un seul espace entre les mots */
			if (in_space && j > 0)
				result[j++] = ' ';
			result[j++] = str[i];
			in_space = 0;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

int	is_special_char(const char *s, char c, int type)
{
	/* Vérifier si une chaîne contient uniquement des $ */
	if (type == 1)
	{
		if (!s || !*s)
			return (0);
		
		while (*s)
		{
			if (*s != '$')
				return (0);
			s++;
		}
		return (1);
	}
	/* Vérifier si c est un caractère d'espacement */
	return (c == ' ' || c == '\t' || c == '\n');
}

int	contains_whitespace(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
			return (1);
		i++;
	}
	return (0);
}

char *expand_heredoc_line(char *line, t_shell_state *shell)
{
    char *result;
    int i, j;
    int len = strlen(line);
    char *expanded_var;
    char var_name[256];
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    /* Allouer suffisamment d'espace pour le résultat */
    result = ft_malloc((len * 2) + 1); /* Estimation généreuse */
    if (!result)
        return (NULL);
    
    i = 0;
    j = 0;
    
    while (i < len)
    {
        /* Gérer les guillemets mais les conserver dans la sortie */
        if (line[i] == '\'')
        {
            result[j++] = line[i++];
            in_single_quotes = !in_single_quotes;
            continue;
        }
        else if (line[i] == '"')
        {
            result[j++] = line[i++];
            in_double_quotes = !in_double_quotes;
            continue;
        }
        
        /* Expansion des variables (même à l'intérieur des quotes comme dans bash) */
        if (line[i] == '$' && i + 1 < len && (isalpha(line[i+1]) || line[i+1] == '_'))
        {
            int var_start = i + 1;
            int var_len = 0;
            
            /* Capturer le nom de la variable */
            while (var_start + var_len < len && 
                   (isalnum(line[var_start + var_len]) || line[var_start + var_len] == '_'))
            {
                var_len++;
            }
            
            strncpy(var_name, line + var_start, var_len);
            var_name[var_len] = '\0';
            
            /* Récupérer la valeur de la variable */
            expanded_var = get_env_value_list(shell->env, var_name);
            
            if (expanded_var)
            {
                strcpy(result + j, expanded_var);
                j += strlen(expanded_var);
            }
            
            i = var_start + var_len;
        }
        /* Gérer le cas spécial $? pour le code de sortie */
        else if (line[i] == '$' && i + 1 < len && line[i+1] == '?')
        {
            char *status_str = ft_itoa(shell->last_exit_status);
            if (status_str)
            {
                strcpy(result + j, status_str);
                j += strlen(status_str);
            }
            i += 2; /* Sauter $? */
        }
        else
        {
            /* Copier le caractère tel quel */
            result[j++] = line[i++];
        }
    }
    
    result[j] = '\0';
    return (result);
}
