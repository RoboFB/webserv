# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/10 10:21:00 by rgohrig           #+#    #+#              #
#    Updated: 2026/07/03 16:17:13 by rgohrig          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                             C++ MAKEFILE
# **************************************************************************** #

# ----------------------------- GENERAL ----------------------------------------

NAME :=				webserv

COMPILER :=			c++
DEBUG_FLAGS :=		-Wshadow -g -fsanitize=address,undefined
PROFILE_FLAGS :=	-pg -g
FAST_FLAGS :=		-O2 #-march=native -O3 -flto # O2 should be enough
LINKER_FLAGS :=		
COMPILE_FLAGS :=	-MMD -MP # MMD & MD for dependencies

CFLAGS :=			-Wall -Werror -Wextra -std=c++17 $(FAST_FLAGS) $(LINKER_FLAGS) $(COMPILE_FLAGS)

DIR_SRC :=			src
SRC :=				main.cpp

DIR_OBJ :=			obj
OBJ :=				$(SRC:%.cpp=$(DIR_OBJ)/%.o)


HEADERS :=			-I ./include

LIBS :=				

DEPENDENCIES := 	$(OBJ:.o=.d)



# ----------------------------- NORMAL -----------------------------------------

# default Rule
all: $(NAME)

$(DIR_OBJ):
	@mkdir -p $(DIR_OBJ)

# Compilation
$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ)
	@mkdir -p $(dir $@)
	@$(COMPILER) $(CFLAGS) $(HEADERS) -o $@ -c $<
	@echo 🏄 $@

# Linking
$(NAME): $(OBJ)
	@$(COMPILER) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "\n   📨🏄📨   $@   ($(CFLAGS))\n"



# ----------------------------- Dependencies -----------------------------------

-include $(DEPENDENCIES)

# ----------------------------- Run --------------------------------------------

run: stop all
	@./$(NAME)

stop:
	@pkill -x $(NAME) > /dev/null 2>&1 && echo "🛑 stopped $(NAME)" || true

# ----------------------------- Clean ------------------------------------------


clean:
	@rm -rf $(DIR_OBJ)
	@echo 🧹 cleaned all objects

fclean: clean
	@rm -f $(NAME)
	@echo 🧹🧹🧹 cleaned $(NAME)

re: fclean all

# alternative re for parallel builds
# re:
# 	@$(MAKE) --no-print-directory fclean
# 	@$(MAKE) --no-print-directory all

# ----------------------------- Debug ------------------------------------------

debug: fclean
debug: CFLAGS += $(DEBUG_FLAGS)
debug: CFLAGS := $(filter-out $(FAST_FLAGS),$(CFLAGS))
debug:
	@$(COMPILER) $(CFLAGS) $(HEADERS) -o $(NAME) $(addprefix $(DIR_SRC)/,$(SRC)) $(LIBS)
	@echo "\n   🐞🐞🐞 DEBUG $(NAME)   ($(CFLAGS))\n"
	@./$(NAME)

# ----------------------------- Profile ----------------------------------------

profile: CFLAGS += $(PROFILE_FLAGS)
# profile: CFLAGS := $(filter-out $(FAST_FLAGS),$(CFLAGS))
profile:
	@$(COMPILER) $(CFLAGS) $(HEADERS) -o $(NAME) $(addprefix $(DIR_SRC)/,$(SRC)) $(LIBS)
	@echo "\n   📊📊📊 PROFILE $(NAME)   ($(CFLAGS))\n"


.PHONY: all clean fclean re debug profile stop run
