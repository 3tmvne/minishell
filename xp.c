/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xp.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ozemrani <ozemrani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 15:00:00 by aregragu          #+#    #+#             */
/*   Updated: 2025/08/11 13:54:22 by ozemrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/**
 * @brief Récupère la valeur d'une variable d'environnement.
 *
 * Gère le cas spécial de `$?` qui doit retourner le dernier code de sortie.
 * Sinon, recherche la variable dans l'environnement.
 *
 * @param name Le nom de la variable à rechercher.
 * @param env Le tableau de chaînes de l'environnement.
 * @param last_exit_status Le code de sortie de la dernière commande.
 * @return char* La valeur de la variable. Doit être libérée par l'appelant,
 * sauf si elle provient de `get_env_value` qui peut retourner `NULL`.
 */
static char	*get_var_value(const char *name, char **env, int last_exit_status)
{
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(last_exit_status));
	return (get_env_value(name, env));
}

/**
 * @brief Expanse les variables dans la valeur d'un token.
 *
 * Parcourt la chaîne `value` et remplace chaque occurrence de `$VAR` ou `$?`
 * par sa valeur correspondante, sauf si le token est entre guillemets simples.
 *
 * @param value La chaîne de caractères originale du token.
 * @param quote Le type de guillemets du token (SQUOTES, DQUOTES, NQUOTES).
 * @param env L'environnement pour la recherche de variables.
 * @param last_exit_status Le code de sortie pour l'expansion de `$?`.
 * @return char* Une nouvelle chaîne de caractères avec les variables expansées.
 *         L'appelant est responsable de la libération de cette mémoire.
 */
char	*expand_token_value(const char *value, t_quote_type quote, char **env,
		int last_exit_status)
{
	int		i;
	int		start;
	char	*result;
	char	*tmp;
	char	*old;
	char	*var_name;
	char	*var_value;

	i = 0;
	start = 0;
	result = ft_strdup("");
	while (value[i])
	{
		// Détecte un '$' qui n'est pas dans des guillemets simples
		// et qui est suivi par des caractères valides pour un nom de variable.
		if (value[i] == '$' && quote != SQUOTES && value[i + 1]
			&& (ft_isalnum(value[i + 1]) || value[i + 1] == '_'
				|| value[i + 1] == '?'))
		{
			// 1. Ajoute la partie de la chaîne avant le '$'
			if (i > start)
			{
				tmp = ft_substr(value, start, i - start); // Extrait la sous-chaîne par exemple HOME
				if (!tmp)
					return (NULL); // Gestion d'erreur si l'allocation échoue
				old = result;
				result = ft_strjoin(result, tmp);
				free(old);
				free(tmp);
			}
			// 2. Extrait le nom de la variable
			int j = i + 1;
			if (value[j] == '?')
				j++;
			else
				while (value[j] && (ft_isalnum(value[j]) || value[j] == '_'))
					j++;
			var_name = ft_substr(value, i + 1, j - i - 1);
			// 3. Récupère la valeur de la variable
			var_value = get_var_value(var_name, env, last_exit_status);
			if (!var_value)
				var_value = ft_strdup(""); // Si la variable n'existe pas, utilise une chaîne vide
			// 4. Ajoute la valeur de la variable au résultat
			old = result;
			result = ft_strjoin(result, var_value);
			free(old);
			free(var_name);
			free(var_value);
			// 5. Met à jour les indices pour continuer après la variable expansée
			start = j;
			i = j - 1;
		}
		i++;
	}
	// Ajoute le reste de la chaîne après la dernière variable
	if (value[start])
	{
		tmp = ft_strdup(value + start);
		old = result;
		result = ft_strjoin(result, tmp);
		free(old);
		free(tmp);
	}
	return (result);
}

/**
 * @brief Parcourt une liste de tokens et expanse les variables pour chacun.
 *
 * Itère sur chaque token de la liste. Si un token est de type `WORD` et a une
 * valeur, il appelle `expand_token_value` pour remplacer les variables.
 * La valeur originale du token est libérée et remplacée par la nouvelle
 * chaîne expansée.
 *
 * @param tokens Le premier token de la liste chaînée.
 * @param env L'environnement.
 * @param last_exit_status Le dernier code de sortie.
 * @return t_token* Le pointeur vers le début de la liste de tokens mise à jour.
 */
t_token	*expand_tokens(t_token *tokens, char **env, int last_exit_status)
{
	t_token	*cur;
	char	*expanded_value;

	cur = tokens;
	while (cur)
	{
		// L'expansion ne s'applique qu'aux tokens de type WORD
		if (cur->type == WORD && cur->value)
		{
			expanded_value = expand_token_value(cur->value, cur->quote, env,
					last_exit_status);
			free(cur->value);
			cur->value = expanded_value;
		}
		cur = cur->next;
	}
	return (tokens);
}
