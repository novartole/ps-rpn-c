ifeq ($(origin CC),default)
	CC = gcc
endif

CFLAGS += -Wall -Wextra

SRC_DIR ?= src
OUT_DIR ?= build
CSRC = main.c numbers.c parse.c string.c tokens.c units.c
COBJ = $(addprefix $(OUT_DIR)/,$(CSRC:.c=.o))

.PHONY: all
all: $(OUT_DIR)/main.x

$(OUT_DIR)/main.x: $(COBJ)
	$(CC) -o $@ $(LDFLAGS) $^

$(COBJ): $(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)

.PHONY: run
run:
	@$(OUT_DIR)/main.x

.PHONY: rebuild
rebuild: clean all
