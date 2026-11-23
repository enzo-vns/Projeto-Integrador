CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I. -Ibackend -Itelas

UNAME_S := $(shell uname -s 2>/dev/null)
ifneq ($(UNAME_S),Darwin)
    LDLIBS += -lcrypto
endif

OBJDIR = obj
BINDIR = bin

SRCS = main.c \
       backend/backend.c \
       telas/telas.c

OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

ifeq ($(OS),Windows_NT)
    TARGET = sistema_recanto.exe
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RMDIR = rmdir /S /Q
    RM = del /Q /F
else
    TARGET = sistema_recanto
    MKDIR = mkdir -p $(1)
    RMDIR = rm -rf
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
	@echo "\n>>> Sistema compilado com sucesso: ./$@\n"

$(OBJDIR)/%.o: %.c backend/backend.h telas/telas.h
	@$(call MKDIR, $(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	@$(RMDIR) $(OBJDIR)
	@$(RM) $(TARGET)
	@echo ">>> Arquivos compilados limpos com sucesso."

test:
	@$(call MKDIR, $(OBJDIR))
	$(CC) $(CFLAGS) tests/test_regras.c backend/backend.c telas/telas.c -o $(OBJDIR)/test_regras $(LDLIBS)
	./$(OBJDIR)/test_regras

.PHONY: all run clean test
