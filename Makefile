CC = cc -g
CFLAGS = -Wall -Werror -Wextra -I. -fPIE
READLINE = -lreadline
SRC = main.c parsing/tokenize.c parsing/syntax_base.c \
	parsing/parser.c parsing/syntax_tokens.c parsing/g_c.c \
	parsing/tokenize_utils.c parsing/tokenize_utils1.c parsing/tokenize_utils2.c \
	executing/execution.c executing/handle_signals.c executing/redirection.c \
	executing/pipes_utils2.c executing/pipes_utils.c executing/sherch_path.c \
	executing/heredoc.c executing/pipes.c executing/heredoc_utils.c \
	built-in/echo.c built-in/unset.c parsing/parser_utils.c \
	built-in/cd.c built-in/exit.c built-in/env_utils.c \
	built-in/export.c built-in/export_utils.c built-in/env.c \
	built-in/pwd.c built-in/export_utils2.c expand/expand.c \
	expand/expand_heredoc.c  parsing/syntax_tokens2.c\
	expand/expand_merge.c expand/expand_quote.c \
	expand/expand_split.c expand/expand_word.c expand/expand_token.c \
	expand/expand_utils.c
OBJ_DIR = ./obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a
NAME = minishell

all: $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJ)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBFT) $(READLINE)

$(OBJ_DIR)/%.o: %.c minishell.h
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@rm -rf $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

valgrind: re clean
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=readline.supp --gen-suppressions=all  --track-fds=yes ./minishell

.PHONY: all re clean fclean
