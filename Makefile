# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/10 10:21:00 by rgohrig           #+#    #+#              #
#    Updated: 2026/11/17 15:47:39 by rgohrig          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                             C++ MAKEFILE
# **************************************************************************** #

# ----------------------------- GENERAL ----------------------------------------

NAME :=				webserv

# only the top-level make sets -j, sub-makes inherit it via the jobserver
ifeq ($(MAKELEVEL),0)
MAKEFLAGS +=		-j$(shell nproc)
endif
MAKEFLAGS +=		--no-print-directory

COMPILER :=			c++
PROFILE_FLAGS :=	-pg -g
FAST_FLAGS :=		-O2
LINKER_FLAGS :=
COMPILE_FLAGS :=	-MMD -MP # MMD & MD for dependencies

CFLAGS :=			-Wall -Werror -Wextra -std=c++17 $(FAST_FLAGS) $(LINKER_FLAGS) $(COMPILE_FLAGS)

DIR_SRC :=			src
SRC :=				utils/CloseFd.cpp utils/logging.cpp utils/Signals.cpp testing.cpp server/AllServer.cpp \
					server/Location.cpp server/Server.cpp ConfigParser/ConfigParseException.cpp \
					ConfigParser/check_args.cpp ConfigParser/fill_context.cpp ConfigParser/fill_default.cpp \
					ConfigParser/helper_parsing.cpp ConfigParser/inherit.cpp ConfigParser/lexer.cpp \
					EpollHandler/ConnectionHandler.cpp EpollHandler/EpollHandler.cpp EpollHandler/SocketFdHandler.cpp \
					EpollHandler/CgiHandler.cpp http/Methods.cpp http/Syntax.cpp main.cpp messages/Header.cpp messages/Message.cpp \
					messages/Request.cpp messages/RequestHandlers.cpp messages/RequestParser.cpp messages/RequestParserBody.cpp \
					messages/RequestParserHead.cpp messages/Response.cpp messages/decode.cpp routing/Resolver.cpp

DIR_OBJ :=			obj
OBJ :=				$(SRC:%.cpp=$(DIR_OBJ)/%.o)


DEBUG_NAME :=		$(NAME)_debug
DEBUG_FLAGS :=		$(filter-out $(FAST_FLAGS),$(CFLAGS)) -Wshadow -g -fsanitize=address,undefined -D LOG_LVL=15
DEBUG_OBJ :=		$(SRC:%.cpp=$(DIR_OBJ)/%_debug.o)



HEADERS :=			-I ./include -I ./include/utils -I ./include/server -I ./include/ConfigParser \
					-I ./include/message -I ./include/routing -I ./include/cgi -I ./include/http \
					-I ./include/EpollHandler

LIBS :=				-lstdc++

DEPENDENCIES := 	$(OBJ:.o=.d) $(DEBUG_OBJ:.o=.d)



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


clean:
	@rm -rf $(DIR_OBJ)
	@echo 🧹 cleaned all objects

fclean: clean debug_fclean
	@rm -f $(NAME)
	@echo 🧹🧹🧹 cleaned $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re

# ----------------------------- Debug ------------------------------------------

both: all debug_all

debug_all: $(DEBUG_NAME)

# Compilation
$(DIR_OBJ)/%_debug.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ)
	@mkdir -p $(dir $@)
	@$(COMPILER) $(DEBUG_FLAGS) $(HEADERS) -o $@ -c $<
	@echo 🐞DEBUG $@

# Linking
$(DEBUG_NAME): $(DEBUG_OBJ)
	@$(COMPILER) $(DEBUG_FLAGS) -o $@ $^ $(LIBS)
	@echo "\n   🐞🐞🐞DEBUG   $@   ($(DEBUG_FLAGS))\n"

debug_fclean: clean
	@rm -f $(DEBUG_NAME)
	@echo 🧹 cleaned $(DEBUG_NAME)


debug: debug_run
debug_run: debug_stop debug_all
	@./$(DEBUG_NAME) -t

debug_stop:
	@pkill -x $(DEBUG_NAME) > /dev/null 2>&1 && echo "🛑 stopped $(DEBUG_NAME)" || true

.PHONY: both debug_all debug_fclean debug debug_run debug_stop


# ----------------------------- Clangd -----------------------------------------

compile_commands:
	@(MAKEFLAGS= compiledb -n make && echo "👀 compile_commands.json updated") || echo "👀 compiledb failed"

# ----------------------------- Phony ------------------------------------------

.PHONY: compile_commands
