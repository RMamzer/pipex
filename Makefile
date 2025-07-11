NAME = so_long

CC = cc

C_FLAGS = -Wall -Wextra -Werror

MLX42_DIR = ./MLX42
MLX42 = $(MLX42_DIR)/build/libmlx42.a

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a


OBJS_DIR = objs/
SRCS_DIR = srcs/

BONUS_OBJS_DIR = bonus_objs/
BONUS_SRCS_DIR = bncs/

HEADERS = -I ./include -I $(MLX42_DIR)/include -I $(LIBFT_DIR)

MLX42_LIBS =  $(MLX42) -ldl -lglfw -pthread -lm

SRCS =  

BONUS_SRCS =

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))
.SECONDARY: ${OBJS}

BONUS_OBJS = $(addprefix $(BONUS_OBJS_DIR), $(BONUS_SRCS:.c=.o))
.SECONDARY: ${BONUS_OBJS}


all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)




$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@ $(HEADERS)

$(BONUS_OBJS_DIR)%.o: $(BONUS_SRCS_DIR)%.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@ $(HEADERS)

$(NAME): $(OBJS) $(LIBFT) $(MLX42)
	$(CC) $(C_FLAGS) $(OBJS) $(LIBFT) $(MLX42_LIBS) -o $(NAME)

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(BONUS_OBJS_DIR)
	rm -f .bonus
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(LIBFT)
	rm -rf $(MLX42_DIR)/build

re: fclean all

bonus: .bonus

.bonus: $(BONUS_OBJS) $(LIBFT) $(MLX42)
	$(CC) $(C_FLAGS) $(BONUS_OBJS) $(LIBFT) $(MLX42_LIBS) -o $(NAME)
	touch .bonus

.PHONY: all re clean fclean bonus
