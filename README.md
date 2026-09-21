# Recanto Bosque — Sistema de Gestão Hoteleira (C)

Projeto Integrador desenvolvido em **Linguagem C** para terminal/console, modelado com arquitetura modular dividida entre **Front-end (Telas e Interface com o Usuário)** e **Back-end (Lógica de Negócio, Dados e Regras)**.

O sistema atende a todas as operações de um hotel/pousada: autenticação de funcionários por perfil, mapa de ocupação semanal de chalés, gestão de reservas, check-in, check-out, lançamento de consumo e relatório de auditoria de operações críticas.

---

## 👥 Equipe e Atribuição de Módulos

O projeto unifica o trabalho desenvolvido pelos membros nas branches individuais:

- **Carlos** (`pi-carlos`): Desenvolvimento do **Mapa de Ocupação** semanal dos chalés e controle de status (`telas/tela_mapa.c` e `backend/chales.c`).
- **João** (`pi-joao`): Desenvolvimento do **Painel da Recepção** e máquina de estados para navegação de telas (`telas/tela_recepcao.c`).
- **Rayane** (`pi-rayane`): Desenvolvimento da **Autenticação (Login)** e do módulo de **Auditoria de Operações** (`telas/tela_login.c`, `telas/tela_auditoria.c` e `backend/auditoria.c`).
- **Enzo** (`pi-enzo`): Integração da arquitetura, estruturação modular Front/Back em C, automação com `Makefile` e documentação técnica.

> **Nota:** Os arquivos originais enviados nas branches individuais estão preservados na pasta [`originais/`](originais/) para histórico e referência do professor.

---

## 📁 Arquitetura do Projeto

```text
Projeto-Integrador/
├── Makefile                     # Script de compilação automatizada (macOS, Linux e Windows)
├── README.md                    # Documentação do projeto
├── .gitignore                   # Arquivos ignorados pelo Git
├── main.c                       # Ponto de entrada do sistema
│
├── telas/                       # FRONT-END (Camada de Apresentação e Telas de Console)
│   ├── tela_login.c / .h        # Tela de Autenticação (Rayane)
│   ├── tela_recepcao.c / .h     # Painel principal e máquina de estados (João)
│   ├── tela_mapa.c / .h         # Visualização em tabela ASCII do mapa de ocupação (Carlos)
│   ├── tela_auditoria.c / .h    # Relatório de auditoria com paginação e busca (Rayane)
│   └── tela_operacoes.c / .h    # Subtelas de Check-in, Check-out, Reservas, Hóspedes e Consumo
│
├── backend/                     # BACK-END (Camada de Lógica de Negócio, Estruturas e Dados)
│   ├── auth.c / .h              # Verificação de credenciais e controle de perfis
│   ├── chales.c / .h            # Dados, tipos e atualização de status dos chalés (Carlos)
│   ├── reservas.c / .h          # Gerenciamento de reservas, check-in e check-out
│   ├── hospedes.c / .h          # Cadastro e consulta de hóspedes
│   ├── consumo.c / .h           # Lançamento e extrato de consumo dos chalés
│   └── auditoria.c / .h         # Registro e armazenamento de logs de auditoria (Rayane)
│
├── common/                      # UTILITÁRIOS GLOBAIS
│   └── common.c / .h            # Limpeza de tela (cross-platform), leitura segura e datas
│
└── originais/                   # CÓDIGOS ORIGINAIS DAS BRANCHES
    ├── Gerenciador-chales.c     # Carlos (pi-carlos)
    ├── auditoriadoadm.c         # Rayane (pi-rayane)
    ├── paginainicial.c          # Rayane (pi-rayane)
    └── painelRecep.c            # João (pi-joao)
```

---

## 🚀 Como Compilar e Executar

### Opção 1: Usando Make (Recomendado)

Na pasta raiz do projeto, execute:

```bash
make
```

Para rodar o executável gerado:

```bash
make run
```

Para limpar os arquivos de compilação:
```bash
make clean
```

---

### Opção 2: Compilação Direta com GCC / Clang

Caso não utilize o comando `make`:

- **Linux / macOS:**
  ```bash
  gcc -Wall -Wextra -std=c99 -I. -Icommon -Ibackend -Itelas main.c common/*.c backend/*.c telas/*.c -o sistema_recanto
  ./sistema_recanto
  ```

- **Windows (MinGW):**
  ```cmd
  gcc -Wall -Wextra -std=c99 -I. -Icommon -Ibackend -Itelas main.c common/*.c backend/*.c telas/*.c -o sistema_recanto.exe
  sistema_recanto.exe
  ```

---

## 🔑 Credenciais para Teste

| Usuário | Senha | Perfil | Módulos e Telas Acessadas |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel Geral, Reservas, Check-in/out, Consumo, Mapa de Chalés |
| `ana.governanca` | `123456` | Governança | Status dos chalés, limpeza, manutenção e Mapa |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria completo, Mapa e Painel Geral |
