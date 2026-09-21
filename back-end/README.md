# Recanto Bosque — Back-end em C

Sistema de gestão hoteleira em C desenvolvido para o terminal, integrando os módulos desenvolvidos pelos membros do projeto:
- **Carlos**: Mapa de ocupação semanal e status dos chalés (`chales.c` / `chales.h`).
- **João**: Painel principal da recepção, máquina de estados e fluxo de atendimento (`recepcao.c` / `recepcao.h`).
- **Rayane**: Autenticação de usuários/funcionários e relatório de auditoria de operações com paginação e busca (`auth.c` / `auditoria.c`).
- Os códigos-fonte originais enviados nas branches individuais estão preservados na pasta [`originais/`](originais/).

---

## Estrutura de Diretórios

```text
back-end/
├── Makefile                # Script de compilação automatizada
├── README.md               # Instruções deste módulo
├── include/                # Headers (.h) com tipos e protótipos
│   ├── auditoria.h         # Módulo de Auditoria (Rayane)
│   ├── auth.h              # Módulo de Autenticação (Rayane)
│   ├── chales.h            # Módulo de Chalés e Mapa (Carlos)
│   ├── common.h            # Tipos globais e utilitários
│   └── recepcao.h          # Painel e Operações da Recepção (João)
├── src/                    # Implementações (.c)
│   ├── auditoria.c
│   ├── auth.c
│   ├── chales.c
│   ├── common.c
│   ├── main.c              # Ponto de entrada integrado
│   └── recepcao.c
└── originais/              # Arquivos originais das branches individuais
    ├── Gerenciador-chales.c
    ├── auditoriadoadm.c
    ├── paginainicial.c
    └── painelRecep.c
```

---

## Como Compilar e Rodar

### Opção 1: Usando Make (Linux, macOS ou Windows com MinGW)

No terminal, acesse a pasta `back-end/` e execute:

```bash
cd back-end
make
```

Para rodar o executável gerado:

```bash
make run
```

Ou diretamente:

- No Linux / macOS:
  ```bash
  ./bin/sistema_recanto
  ```
- No Windows:
  ```cmd
  bin\sistema_recanto.exe
  ```

Para limpar os arquivos compilados:
```bash
make clean
```

---

### Opção 2: Compilação Direta com GCC / Clang

Se não tiver o utilitário `make`:

```bash
cd back-end
gcc -Wall -Wextra -std=c99 -Iinclude src/*.c -o bin/sistema_recanto
./bin/sistema_recanto
```

---

## Usuários de Teste

| Usuário | Senha | Perfil | Acesso |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel da Recepção, Mapa, Check-in/out, Consumo, Reservas, Hóspedes |
| `ana.governanca` | `123456` | Governança | Gerenciamento de limpeza/manutenção e Mapa de chalés |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria completo e Painel de Recepção |
