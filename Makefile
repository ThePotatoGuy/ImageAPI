TARGET	:= ImageAPI
CC		:= gcc
CFLAGS	:= -Wall -std=c99 -O2

SRC			:= $(shell find src -name '*.c')
OBJS		:= $(patsubst src/%.c,src/%.o,$(SRC))

.PHONY: \
	all		\
	clean	\

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o build/$@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJS) build