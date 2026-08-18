# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/10 10:21:00 by rgohrig           #+#    #+#              #
#    Updated: 2026/08/18 15:49:22 by rgohrig          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                             C++ MAKEFILE
# **************************************************************************** #

# ----------------------------- GENERAL ----------------------------------------

NAME :=				webserv

COMPILER :=			c++
PROFILE_FLAGS :=	-pg -g
FAST_FLAGS :=		-O2 #-march=native -O3 -flto # O2 should be enough
LINKER_FLAGS :=		
COMPILE_FLAGS :=	-MMD -MP # MMD & MD for dependencies

CFLAGS :=			-Wall -Werror -Wextra -std=c++17 $(FAST_FLAGS) $(LINKER_FLAGS) $(COMPILE_FLAGS)

DIR_SRC :=			src
SRC :=				$(shell find $(DIR_SRC) -type f -name '*.cpp' | sed 's|^$(DIR_SRC)/||')

DIR_OBJ :=			obj
OBJ :=				$(SRC:%.cpp=$(DIR_OBJ)/%.o)


DEBUG_NAME :=		$(NAME)_debug
DEBUG_FLAGS :=		$(filter-out $(FAST_FLAGS),$(CFLAGS)) -Wshadow -g -fsanitize=address,undefined -D LOG_LVL=15
DEBUG_OBJ :=		$(SRC:%.cpp=$(DIR_OBJ)/%_debug.o)



HEADERS :=			-I ./include

LIBS :=				

DEPENDENCIES := 	$(OBJ:.o=.d)



# ----------------------------- NORMAL -----------------------------------------

# default Rule
all: compile_commands $(NAME)

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

fclean: clean debug_fclean
	@rm -f $(NAME)
	@echo 🧹🧹🧹 cleaned $(NAME)

re: fclean all

# alternative re for parallel builds
# re:
# 	@$(MAKE) --no-print-directory fclean
# 	@$(MAKE) --no-print-directory all

# ----------------------------- Debug ------------------------------------------

both: all debug_all

debug_all: compile_commands $(DEBUG_NAME)

# Compilation
$(DIR_OBJ)/%_debug.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ)
	@mkdir -p $(dir $@)
	@$(COMPILER) $(DEBUG_FLAGS) $(HEADERS) -o $@ -c $<
	@echo 🐞DEBUG $@

# Linking
$(DEBUG_NAME): $(DEBUG_OBJ)
	@$(COMPILER) $(DEBUG_FLAGS) -o $@ $^ $(LIBS)
	@echo "\n   🐞🐞🐞DEBUG   $@   ($(DEBUG_FLAGS))\n"

debug_fclean:
	@rm -f $(DEBUG_NAME)
	@echo 🧹 cleaned $(DEBUG_NAME)


debug: debug_run
debug_run: debug_stop debug_all
	@./$(DEBUG_NAME) -t

debug_stop:
	@pkill -x $(DEBUG_NAME) > /dev/null 2>&1 && echo "🛑 stopped $(DEBUG_NAME)" || true

.PHONY: both debug_all debug_fclean debug debug_run debug_stop 

# ----------------------------- Profile ----------------------------------------

profile: CFLAGS += $(PROFILE_FLAGS)
# profile: CFLAGS := $(filter-out $(FAST_FLAGS),$(CFLAGS))
profile:
	@$(COMPILER) $(CFLAGS) $(HEADERS) -o $(NAME) $(addprefix $(DIR_SRC)/,$(SRC)) $(LIBS)
	@echo "\n   📊📊📊 PROFILE $(NAME)   ($(CFLAGS))\n"



# ----------------------------- Clangd -----------------------------------------

compile_commands:
	@(compiledb -n make && echo "👀 compile_commands.json updated") || echo "👀 compiledb failed"

# ----------------------------- Phony ------------------------------------------

.PHONY: all clean fclean re profile stop run compile_commands
