CC ?= cc
CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
TEST_SCORE := $(BUILD_DIR)/test_score
TEST_BOOT := $(BUILD_DIR)/test_bootblock

.PHONY: all check clean

all: $(TEST_SCORE) $(TEST_BOOT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_SCORE): tests/test_score.c src/core/score.c include/amiheuristics/amiheuristics.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_score.c src/core/score.c

$(TEST_BOOT): tests/test_bootblock.c src/boot/bootblock.c include/amiheuristics/bootblock.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_bootblock.c src/boot/bootblock.c

check: $(TEST_SCORE) $(TEST_BOOT)
	./$(TEST_SCORE)
	./$(TEST_BOOT)
	python3 tools/check_m0.py

clean:
	rm -rf $(BUILD_DIR)
