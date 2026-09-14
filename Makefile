CC ?= cc
CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
TEST_SCORE := $(BUILD_DIR)/test_score
TEST_BOOT := $(BUILD_DIR)/test_bootblock
TEST_M68K := $(BUILD_DIR)/test_m68k
TEST_EXEC_LVO := $(BUILD_DIR)/test_exec_lvo
TEST_HUNK := $(BUILD_DIR)/test_hunk

.PHONY: all check clean

all: $(TEST_SCORE) $(TEST_BOOT) $(TEST_M68K) $(TEST_EXEC_LVO) $(TEST_HUNK)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_SCORE): tests/test_score.c src/core/score.c include/amiheuristics/amiheuristics.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_score.c src/core/score.c

$(TEST_BOOT): tests/test_bootblock.c src/boot/bootblock.c src/m68k/decode.c src/amiga/exec_lvo.c include/amiheuristics/bootblock.h include/amiheuristics/m68k.h include/amiheuristics/exec_lvo.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_bootblock.c src/boot/bootblock.c src/m68k/decode.c src/amiga/exec_lvo.c

$(TEST_M68K): tests/test_m68k.c src/m68k/decode.c include/amiheuristics/m68k.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_m68k.c src/m68k/decode.c

$(TEST_EXEC_LVO): tests/test_exec_lvo.c src/amiga/exec_lvo.c include/amiheuristics/exec_lvo.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_exec_lvo.c src/amiga/exec_lvo.c

$(TEST_HUNK): tests/test_hunk.c src/file/hunk.c src/m68k/decode.c include/amiheuristics/hunk.h include/amiheuristics/m68k.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_hunk.c src/file/hunk.c src/m68k/decode.c

check: $(TEST_SCORE) $(TEST_BOOT) $(TEST_M68K) $(TEST_EXEC_LVO) $(TEST_HUNK)
	./$(TEST_SCORE)
	./$(TEST_BOOT)
	./$(TEST_M68K)
	./$(TEST_EXEC_LVO)
	./$(TEST_HUNK)
	python3 tools/check_m0.py

clean:
	rm -rf $(BUILD_DIR)
