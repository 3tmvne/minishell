CC = cc -g
CFLAGS = -Wall -Werror -Wextra -I.
READLINE = -lreadline
SRC = main.c tokenize.c input.c syntax_base.c \
	parser.c syntax_tokens.c heredoc.c \
	execution.c redirection.c  \
	pipes.c env_utils.c g_b.c \
	built-in/echo.c built-in/env.c \
	built-in/cd.c built-in/exit.c \
	built-in/export.c built-in/pwd.c \
	built-in/unset.c \
	expand/expand_utils1.c expand/expand_utils2.c \
	expand/expand.c

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

.PHONY: all re clean fclean test_parser debug_parser
.SECONDARY: $(OBJ)