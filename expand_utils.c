#include "minishell.h"

/**
 * Concatenate tokens for assignment value, skipping empty quoted tokens.
 * For example with: "export zzz=fffff""ddddddd", 
 * it should include all non-empty tokens to produce "fffffddddddd"
 * tokens: linked list of t_token (WORD tokens, possibly quoted)
 * Returns a newly allocated string.
 */
char *concat_assignment_value(t_token *tokens)
{
    // Special case for export command: we're handling "zzz=fffff""ddddddd" where
    // the first token is "zzz=fffff", the second is an empty quoted token, and the third is "ddddddd"
    
    size_t total_len = 0;
    t_token *cur = tokens;
    
    // First pass: calculate total length for all non-empty tokens
    while (cur) {
        // Skip tokens that contain assignment operators (we only want values)
        if (cur->type == WORD && ft_strchr(cur->value, '='))
            continue;
            
        // Skip empty quoted tokens (like "" or '')
        if (cur->type == WORD && 
            !((cur->quote == DQUOTES || cur->quote == SQUOTES) && 
             (!cur->value || cur->value[0] == '\0'))) {
            total_len += ft_strlen(cur->value);
        }
        
        cur = cur->next;
    }
    // Allocate memory for the result
    char *result = malloc(total_len + 1);
    if (!result)
        return NULL;
    result[0] = '\0';
    
    // Second pass: concatenate all non-empty tokens
    cur = tokens;
    while (cur) {
        // Skip tokens with assignment operators (we only want values)
        if (cur->type == WORD && ft_strchr(cur->value, '=')) {
            cur = cur->next;
            continue;
        }
        
        // Skip empty quoted tokens (like "" or '')
        if (cur->type == WORD && 
            !((cur->quote == DQUOTES || cur->quote == SQUOTES) && 
             (!cur->value || cur->value[0] == '\0'))) {
            ft_strlcat(result, cur->value, total_len + 1);
        }
        cur = cur->next;
    }
    return result;
}
