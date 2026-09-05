CC := gcc

CPPFLAGS := -Iinclude
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic
LDLIBS := -lcmocka

BUILD_DIR := build
APP := $(BUILD_DIR)/gerenciador_processos
MAIN_OBJECT := $(BUILD_DIR)/src/main.o
LIBRARY_SOURCES := $(filter-out src/main.c,$(wildcard src/*.c))
LIBRARY_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIBRARY_SOURCES))
APP_OBJECTS := $(MAIN_OBJECT) $(LIBRARY_OBJECTS)
TEST_SOURCES := $(wildcard tests/unit/test_*.c tests/feature/test_*.c)
TEST_BINS := $(patsubst %.c,$(BUILD_DIR)/%,$(TEST_SOURCES))

.PHONY: build run test clean

build: $(APP)

run: $(APP)
	./$(APP)

test: $(TEST_BINS)
	@for test_binary in $(TEST_BINS); do \
		echo "Running $$test_binary"; \
		./$$test_binary || exit $$?; \
	done

clean:
	rm -rf $(BUILD_DIR)

$(APP): $(APP_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/tests/%: tests/%.c $(LIBRARY_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIBRARY_OBJECTS) -o $@ $(LDLIBS)

$(BUILD_DIR)/src/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
