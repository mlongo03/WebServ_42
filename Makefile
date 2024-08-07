NAME = webserv

SRC = $(wildcard srcs/*.cpp)

HDRS = $(wildcard includes/*.hpp)

OBJS = $(SRC:srcs/%.cpp=obj/%.o)

FLAGS := -Wall -Werror -Wextra -std=c++98 -I includes

RED = \033[1;31m

GREEN = \033[1;32m

YELLOW = \033[1;33m

DEFAULT = \033[0m

all: $(NAME)

obj/%.o: srcs/%.cpp
	@(c++ $(FLAGS) -c $< -o $@)

$(NAME): $(OBJS) $(HDRS)
	@(c++ $(FLAGS) $(OBJS) -o $(NAME))
	@echo "$(GREEN)$(NAME) created!$(DEFAULT)"

clean:
	@rm -f $(OBJS)
	@echo "$(YELLOW)object files deleted!$(DEFAULT)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)all deleted!$(DEFAULT)"

re: clean fclean all
