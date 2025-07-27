CC = cc 
CFLAGS = -Wall -Werror -Wextra
READLINE = -lreadline
SRC = main.c cd.c echo.c env.c export.c pwd.c signal.c unset.c utils.c \
	execution.c parsing.c split_line.c
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