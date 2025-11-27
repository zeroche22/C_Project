CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -pthread
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -pthread
LDFLAGS = -pthread
GTEST_LIBS = -lgtest -lgtest_main

SERVER_SRC = src/server.c
COMMON_SRC = \
src/client_handler.c \
src/storage.c \
src/analysis.c \
src/key_decoder.c

SRC = $(SERVER_SRC) $(COMMON_SRC)

OBJ = $(SRC:.c=.o)
COMMON_OBJ = $(COMMON_SRC:.c=.o)

TARGET = json_server
TEST_TARGET = json_server_tests

TEST_SRC = tests/test_all.cpp

TEST_OBJ = $(TEST_SRC:.cpp=.o)

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(TEST_TARGET): $(COMMON_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(COMMON_OBJ) $(TEST_OBJ) $(GTEST_LIBS) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

run: all
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean run test
