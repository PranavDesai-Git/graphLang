# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude -g

# Directories
SRCDIR = src
OBJDIR = out
INCDIR = include

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET = $(OBJDIR)/graphLang
PLUGIN = $(OBJDIR)/CoreMath.so

.PHONY: all clean run plugin

all: $(TARGET) plugin

plugin: $(PLUGIN)

$(PLUGIN): plugins/CoreMath.c | $(OBJDIR)
	$(CC) -shared -fPIC -Iinclude $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -ldl -lreadline

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

run: all
	./$(TARGET)

compiledb: clean
	bear -- make all

