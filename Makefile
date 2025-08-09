CC = cc -g
CFLAGS = -Wall -Werror -Wextra -I.
READLINE = -lreadline
SRC = main.c tokenize.c input.c parser.c syntax_tokens.c syntax_base.c execution.c built-in/echo.c
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