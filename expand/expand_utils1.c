/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aregragu <aregragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 04:33:26 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/20 10:54:11 by aregragu         ###   ########.fr       */
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