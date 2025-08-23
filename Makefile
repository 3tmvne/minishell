CC = cc -g
CFLAGS = -Wall -Werror -Wextra -I. -fPIE
READLINE = -lreadline
SRC = main.c parsing/tokenize.c parsing/syntax_base.c \
	parsing/parser.c parsing/syntax_tokens.c parsing/g_b.c \
	parsing/tokenize_utils.c executing/execution.c \
	executing/handle_signals.c executing/redirection.c \
	executing/pipes_utils2.c executing/pipes_utils.c \
	executing/heredoc.c executing/pipes.c built-in/env.c \
	built-in/echo.c built-in/unset.c \
	built-in/cd.c built-in/exit.c built-in/env_utils.c \
	built-in/export.c built-in/export_utils.c built-in/export_utils2.c \
	built-in/pwd.c expand/expand.c \
	expand/expand_utils1.c expand/expand_utils2.c \
	expand/expand_utils3.c expand/expand_utils4.c \
	expand/expand_utils6.c expand/expand_utils5.c
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

.PHONY: all re clean fclean