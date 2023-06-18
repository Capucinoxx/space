CC = g++
CFLAGS = -std=c++20 -Wall -Werror -Iinclude

SRCDIR = src
TESTDIR = tests
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.cpp)
TESTSRCS = $(wildcard $(TESTDIR)/*.cpp)

OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SRCS))
TESTOBJS = $(patsubst $(TESTDIR)/%.cpp,$(BINDIR)/%.o,$(TESTSRCS))

server: $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ -lpqxx -lpq

test_server: $(TESTOBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -lpqxx -lpq

$(BINDIR)/%.o: $(TESTDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)/*.o $(BINDIR)/server $(BINDIR)/test_server
