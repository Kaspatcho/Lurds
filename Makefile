CC := gcc

CPPFLAGS := -Iinclude
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic
LDLIBS := -lcmocka

BUILD_DIR := build
APP := $(BUILD_DIR)/gerenciador_processos
APP_CPPFLAGS := $(CPPFLAGS) -DIS_TESTING=0
TEST_CPPFLAGS := $(CPPFLAGS) -DIS_TESTING=1
HEADERS := $(wildcard include/*.h)
MAIN_OBJECT := $(BUILD_DIR)/app/src/main.o
LIBRARY_SOURCES := $(filter-out src/main.c,$(wildcard src/*.c))
APP_LIBRARY_OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/app/src/%.o,$(LIBRARY_SOURCES))
TEST_LIBRARY_OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/test/src/%.o,$(LIBRARY_SOURCES))
APP_OBJECTS := $(MAIN_OBJECT) $(APP_LIBRARY_OBJECTS)
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

$(BUILD_DIR)/tests/%: tests/%.c $(TEST_LIBRARY_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) $< $(TEST_LIBRARY_OBJECTS) -o $@ $(LDLIBS)

$(BUILD_DIR)/app/src/%.o: src/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(APP_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test/src/%.o: src/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) -c $< -o $@
