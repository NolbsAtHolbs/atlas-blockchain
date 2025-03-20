CC := gcc
CFLAGS := -Wall -Werror -Wextra -pedantic -Wno-deprecated-declarations -std=gn89
AR := ar
ARFLAGS := rc
CPPFLAGS := -I. -Itransaction/ -I../../crypto
LDFLAGS := -L../../crypto
LDLIBS := -lhblk_crypto -lllist -lssl -lcrypto

OBJ_DIR := ./

SRC := $(wildcard ./*.c) $(wildcard transaction/*.c)

OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.c=.o)))

VPATH := $(sort $(dir $(SRC)))

all: libhblk_blockchain.a

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

libhblk_blockchain.a: $(OBJ)
	ar rcs $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f libhblk_blockchain.a

.PHONY: all clean fclean
