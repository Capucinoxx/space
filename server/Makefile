CC := g++
CFLAGS := -std=c++20 -Wall -Werror -iquote ./include
DEPFLAGS = -MMD -MP

SERVER_FLAGS :=  -lpqxx -lpq

SRC_DIR := src
TEST_DIR := tests

BIN_DIR := bin

SERVER := $(BIN_DIR)/server
TESTS := $(BIN_DIR)/test_server
ANGRY := $(BIN_DIR)/angry_server

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
ANGRY_SRCS := $(wildcard $(SRC_DIR)/*.cpp)

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(SERVER)_objs/%.o)
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(TESTS)_objs/%.o)
ANGRY_OBJS := $(ANGRY_SRCS:$(SRC_DIR)/%.cpp=$(ANGRY)_objs/%.o)

DEPS := $(OBJS:.o=.d)
TEST_DEPS := $(TEST_DEPS:.o=.d)
ANGRY_DEPS := $(ANGRY_OBJS:.o=.d)

all: $(SERVER) $(TESTS) $(ANGRY)

.PHONY: all

$(SERVER): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(SERVER_FLAGS)

$(TESTS): $(TEST_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(SERVER_FLAGS)

$(SERVER)_objs/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@ $(SERVER_FLAGS)

$(TESTS)_objs/%.o: $(TEST_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@ $(SERVER_FLAGS)

$(ANGRY)_objs/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	export ASAN_OPTIONS="new_delete_type_mismatch=0"
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

$(ANGRY): CFLAGS += -g3 -fsanitize=address,leak,undefined
$(ANGRY): LDFLAGS += -lasan
$(ANGRY): $(ANGRY_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(SERVER_FLAGS)

clean:
	$(RM) -r $(BIN_DIR)/**/*.o $(BIN_DIR)/server $(BIN_DIR)/test_server

-include $(DEPS)
-include $(TEST_DEPS)
-include $(ANGRY_DEPS)

.PHONY: clean
