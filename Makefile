NAME = pipex

CC = cc
#CHECK NECESSARY FLAGS LATER
C_FLAGS = -g -Wall -Wextra -Werror

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a


OBJS_DIR = objs/
SRCS_DIR = srcs/

BONUS_OBJS_DIR = bonus_objs/
BONUS_SRCS_DIR = bncs/

HEADERS = -I ./include  -I $(LIBFT_DIR)

SRCS = main.c

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))
.SECONDARY: ${OBJS}


all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@ $(HEADERS)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(C_FLAGS) $(OBJS) $(LIBFT) -o $(NAME)

clean:
	rm -rf $(OBJS_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(LIBFT)

re: fclean all

.PHONY: all re clean fclean
