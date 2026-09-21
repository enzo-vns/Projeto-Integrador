CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

ifeq ($(OS),Windows_NT)
    TARGET = $(BINDIR)/sistema_recanto.exe
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = del /Q /F
    RMDIR = rmdir /S /Q
else
    TARGET = $(BINDIR)/sistema_recanto
    MKDIR = mkdir -p $(1)
    RM = rm -f
    RMDIR = rm -rf
endif

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@$(call MKDIR, $(OBJDIR))

$(BINDIR):
	@$(call MKDIR, $(BINDIR))

run: $(TARGET)
	./$(TARGET)

clean:
	@$(RMDIR) $(OBJDIR) $(BINDIR)

.PHONY: all run clean
