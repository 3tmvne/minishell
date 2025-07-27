CC = cc 
CFLAGS = -Wall -Werror -Wextra
READLINE = -lreadline
SRC = main.c  utils.c parsing.c syntaxe.c lexing.c
OBJ_DIR = ./obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
NAME = minishell

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(READLINE) $(CFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o: %.c minishell.h
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all re clean fclean
.SECONDARY: $(OBJ)