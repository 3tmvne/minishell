/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xp.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 15:00:00 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/13 03:08:04 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

char	*expand_token_value(const char *value, t_quote_type quote,
		t_shell_state *shell)
{
	char	*result;
	char	*temp;
	char	*var_name;
	char	*var_value;
	int		i;
	int		j;
	int		var_len;
	int		in_single_quotes;

	// Si le token a été créé à partir de guillemets simples, ne pas expanser
	if (quote == SQUOTES)
		return (ft_strdup(value));
	result = ft_strdup("");
	i = 0;
	in_single_quotes = 0;
	while (value[i])
	{
		// Gérer les guillemets simples - pas d'expansion à l'intérieur
		if (value[i] == '\'' && !in_single_quotes)
		{
			in_single_quotes = 1;
			i++; // Skip opening quote
			continue ;
		}
		if (value[i] == '\'' && in_single_quotes)
		{
			in_single_quotes = 0;
			i++; // Skip closing quote
			continue ;
		}
		// Si on est dans des guillemets simples, copier tel quel
		if (in_single_quotes)
		{
			temp = malloc(ft_strlen(result) + 2);
			if (!temp)
			{
				free(result);
				return (ft_strdup(""));
			}
			ft_strlcpy(temp, result, ft_strlen(result) + 1);
			temp[ft_strlen(result)] = value[i];
			temp[ft_strlen(result) + 1] = '\0';
			free(result);
			result = temp;
			i++;
			continue ;
		}
		// Gérer les guillemets doubles - expansion à l'intérieur,
			// mais ignorer les quotes
		if (value[i] == '"')
		{
			i++; // Skip opening quote
			// Continuer normalement jusqu'à la quote fermante
			while (value[i] && value[i] != '"')
			{
				// Dans les guillemets doubles, on peut expanser les variables
				if (value[i] == '$' && value[i + 1])
				{
					// Cas spécial pour $?
					if (value[i + 1] == '?' && (value[i + 2] == '\0' || value[i
							+ 2] == '"' || !ft_isalnum(value[i + 2])))
					{
						var_value = ft_itoa(shell->last_exit_status);
						temp = ft_strjoin(result, var_value);
						free(result);
						free(var_value);
						result = temp;
						i += 2;
						continue ;
					}
					// Cas normal pour les variables
					if (ft_isalpha(value[i + 1]) || value[i + 1] == '_')
					{
						j = i + 1;
						while (value[j] && (ft_isalnum(value[j])
								|| value[j] == '_'))
							j++;
						var_len = j - i - 1;
						var_name = ft_substr(value, i + 1, var_len);
						var_value = get_env_value_list(shell->env, var_name);
						free(var_name);
						if (var_value)
						{
							temp = ft_strjoin(result, var_value);
							free(result);
							result = temp;
						}
						i = j;
						continue ;
					}
				}
				// Caractère normal dans les guillemets doubles
				temp = malloc(ft_strlen(result) + 2);
				if (!temp)
				{
					free(result);
					return (ft_strdup(""));
				}
				ft_strlcpy(temp, result, ft_strlen(result) + 1);
				temp[ft_strlen(result)] = value[i];
				temp[ft_strlen(result) + 1] = '\0';
				free(result);
				result = temp;
				i++;
			}
			if (value[i] == '"')
			{
				i++; // Skip closing quote
			}
			continue ; // Retour au début de la boucle principale
		}
		// Expansion des variables (en dehors des guillemets)
		if (value[i] == '$' && value[i + 1])
		{
			// Cas spécial pour $?
			if (value[i + 1] == '?' && (value[i + 2] == '\0'
					|| !ft_isalnum(value[i + 2])))
			{
				var_value = ft_itoa(shell->last_exit_status);
				temp = ft_strjoin(result, var_value);
				free(result);
				free(var_value);
				result = temp;
				i += 2;
				continue ;
			}
			// Cas normal pour les variables ($USER, $HOME, etc.)
			if (ft_isalpha(value[i + 1]) || value[i + 1] == '_')
			{
				// Trouver la longueur du nom de variable
				j = i + 1;
				while (value[j] && (ft_isalnum(value[j]) || value[j] == '_'))
					j++;
				var_len = j - i - 1;
				// Extraire le nom de variable
				var_name = ft_substr(value, i + 1, var_len);
				var_value = get_env_value_list(shell->env, var_name);
				free(var_name);
				if (var_value)
				{
					temp = ft_strjoin(result, var_value);
					free(result);
					result = temp;
				}
				// Si variable inexistante, on ne concatène rien (chaîne vide)
				i = j;
				continue ;
			}
		}
		// Caractère normal, l'ajouter au résultat
		temp = malloc(ft_strlen(result) + 2);
		if (!temp)
		{
			free(result);
			return (ft_strdup(""));
		}
		ft_strlcpy(temp, result, ft_strlen(result) + 1);
		temp[ft_strlen(result)] = value[i];
		temp[ft_strlen(result) + 1] = '\0';
		free(result);
		result = temp;
		i++;
	}
	return (result);
}

t_token	*expand_tokens(t_token *tokens, t_shell_state *shell)
{
	t_token	*cur;
	char	*expanded_value;

	cur = tokens;
	while (cur)
	{
		// L'expansion ne s'applique qu'aux tokens de type WORD
		if (cur->type == WORD && cur->value)
		{
			expanded_value = expand_token_value(cur->value, cur->quote, shell);
			free(cur->value);
			cur->value = expanded_value;
		}
		cur = cur->next;
	}
	return (tokens);
}

/**
 * Expansion sélective : traitement spécial pour certaines commandes
 */
t_token	*expand_tokens_selective(t_token *tokens, t_shell_state *shell)
{
	t_token	*cur;
	char	*expanded_value;
	char	*cmd_name;
	int		is_export_no_value;

	if (!tokens || tokens->type != WORD)
		return (tokens);
	cmd_name = tokens->value;
	cur = tokens;
	while (cur)
	{
		// L'expansion ne s'applique qu'aux tokens de type WORD
		if (cur->type == WORD && cur->value)
		{
			is_export_no_value = 0;
			// Cas spécial : export sans valeur (export VAR)
			// Ne pas expanser seulement si c'est export ET que l'argument ne contient pas '='
			if (cur != tokens && ft_strcmp(cmd_name, "export") == 0)
			{
				if (!ft_strchr(cur->value, '='))
					is_export_no_value = 1;
			}
			// Expanser dans tous les cas sauf export sans valeur
			if (!is_export_no_value)
			{
				expanded_value = expand_token_value(cur->value, cur->quote,
						shell);
				free(cur->value);
				cur->value = expanded_value;
			}
		}
		cur = cur->next;
	}
	return (tokens);
}
