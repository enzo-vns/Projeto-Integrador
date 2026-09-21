CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I. -Ibackend -Itelas

OBJDIR = obj
BINDIR = bin

SRCS = main.c \
       backend/common.c \
       backend/chales.c \
       backend/reservas.c \
       backend/hospedes.c \
       backend/consumo.c \
       backend/auditoria.c \
       backend/auth.c \
       telas/tela_login.c \
       telas/tela_recepcao.c \
       telas/tela_mapa.c \
       telas/tela_auditoria.c \
       telas/tela_operacoes.c

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
	$(CC) $(CFLAGS) $^ -o $@
	@echo "\n>>> Sistema compilado com sucesso: ./$@\n"

$(OBJDIR)/%.o: %.c
	@$(call MKDIR, $(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	@$(RMDIR) $(OBJDIR)
	@$(RM) $(TARGET)
	@echo ">>> Arquivos compilados limpos com sucesso."

.PHONY: all run clean
