CC = gcc

SRC = source/main.c source/skript.c

INC = ./include

TARGET_MAIN = ./execution/main
TARGET_SKRIPT = ./execution/skript

OBJ = $(SRC:.c=.o)
FLAG = inih/ini.c -lm

all: $(TARGET_MAIN) $(TARGET_SKRIPT)

$(TARGET_MAIN): main.o 
	$(CC) -o $@ main.o $(INC) 

# Rule to create the skript executable
$(TARGET_SKRIPT): skript.o 
	$(CC) -o $@ skript.o $(INC) 

# Pattern rule for compiling .c files to .o files
%.o: %.c
	$(CC) -c $< $(INC)

# Clean up
clean:
	rm -f $(OBJ) $(TARGET_MAIN) $(TARGET_SKRIPT)