NAME = webserv

TESTING_FOLDER = var/www

SRC = $(wildcard srcs/*.cpp)

HDRS = $(wildcard includes/*.hpp)

OBJS = $(SRC:srcs/%.cpp=obj/%.o)

OBJDIR = obj

FLAGS := -Wall -Werror -Wextra -std=c++98 -I includes

RED = \033[1;31m

GREEN = \033[1;32m

YELLOW = \033[1;33m

DEFAULT = \033[0m

all: $(NAME)

valgrind : $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./webserv config/config.conf

valgrind_safe : $(NAME)
	-valgrind --leak-check=full --show-leak-kinds=all ./webserv config/config.conf || true

obj/%.o: srcs/%.cpp
	@mkdir -p $(@D)
	@(c++ $(FLAGS) -c $< -o $@)

$(NAME): $(OBJS) $(HDRS) permission_set
	@(c++ $(FLAGS) $(OBJS) -o $(NAME))
	@echo "$(GREEN)$(NAME) created!$(DEFAULT)"

permission_set:
	@chmod 000 $(TESTING_FOLDER)/get_test/not_permission_file.txt
	@chmod 000 $(TESTING_FOLDER)/test_notauthorized

permission_unset:
	@chmod 777 $(TESTING_FOLDER)/get_test/not_permission_file.txt
	@chmod 777 $(TESTING_FOLDER)/test_notauthorized

clean: permission_unset
	@rm -f $(OBJS)
	@echo "$(YELLOW)object files deleted!$(DEFAULT)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)all deleted!$(DEFAULT)"

re: clean fclean all
