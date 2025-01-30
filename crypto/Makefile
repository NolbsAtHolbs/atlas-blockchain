CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations
INC := -I.
LIB := libhblk_crypto.a
SRC_DIR := .
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(LIB)

re: fclean all
