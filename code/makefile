CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses


SRC = processus_main.c processus_ui.c process.c
OBJ = $(SRC:.c=.o)


TARGET = mytop

all: deps $(TARGET)


deps:
	@echo "Verification de ncurses..."
	@ldconfig -p | grep -q libncursesw || ldconfig -p | grep -q libncurses || { \
		echo "ncurses n'est pas installe. Installation..."; \
		sudo apt-get update && sudo apt-get install -y libncurses5-dev libncursesw5-dev; \
	}


$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c processus_ui.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
