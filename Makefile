# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude -g

# Directories
SRCDIR = src
OBJDIR = out
INCDIR = include
PLUGINDIR = plugins

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
TARGET = $(OBJDIR)/graphLang

# Plugins
PLUGIN_SRCS = $(wildcard $(PLUGINDIR)/*.c)
PLUGIN_OBJS = $(patsubst $(PLUGINDIR)/%.c, $(OBJDIR)/%.so, $(PLUGIN_SRCS))

.PHONY: all clean run

all: $(TARGET) $(PLUGIN_OBJS)

$(OBJDIR)/%.so: $(PLUGINDIR)/%.c | $(OBJDIR)
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
