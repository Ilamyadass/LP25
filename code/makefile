CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

SRC = ex_main.c ui.c
OBJ = $(SRC:.c=.o)

TARGET = ui_app

all: deps $(TARGET)

deps:
	@echo "verification de ncurses..."
	@ldconfig -p | grep -q libncursesw || { \
		echo "ncurses nest pas installe. Installation..."; \
		sudo apt-get update && sudo apt-get install -y libncurses5-dev libncursesw5-dev; \
	}
	@echo "ncurses est installe."

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c ui.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
