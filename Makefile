CC := gcc

CPPFLAGS := -Iinclude
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic
LDLIBS := -lcmocka

BUILD_DIR := build
APP := $(BUILD_DIR)/gerenciador_processos
APP_OBJECTS := $(BUILD_DIR)/src/main.o $(BUILD_DIR)/src/example.o
UNIT_TEST := $(BUILD_DIR)/tests/unit/test_example
FEATURE_TEST := $(BUILD_DIR)/tests/feature/test_fixture

.PHONY: build run test clean

build: $(APP)

run: $(APP)
	./$(APP)

test: $(UNIT_TEST) $(FEATURE_TEST)
	./$(UNIT_TEST)
	./$(FEATURE_TEST)

clean:
	rm -rf $(BUILD_DIR)

$(APP): $(APP_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

$(UNIT_TEST): tests/unit/test_example.c $(BUILD_DIR)/src/example.o include/example.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(BUILD_DIR)/src/example.o -o $@ $(LDLIBS)

$(FEATURE_TEST): tests/feature/test_fixture.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: %.c include/example.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
