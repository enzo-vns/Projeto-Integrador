# Recanto Bosque — Sistema de Gestão Hoteleira (Back-end em C)

Repositório do **Projeto Integrador**, com foco no desenvolvimento do **Back-end em Linguagem C** (Console / Terminal) para gerenciamento completo das operações do hotel/pousada **Recanto Bosque**.

O sistema contempla todo o fluxo de atendimento hoteleiro: autenticação de funcionários com controle de perfis de acesso, mapa visual semanal de chalés, gestão de reservas, realização de check-in e check-out, lançamento de consumo e relatório de auditoria de operações.

> 📌 **Escopo do Semestre:** Este semestre é dedicado exclusivamente ao desenvolvimento e integração do **Back-end e interface de console em C**. A camada de Front-end Web externa será desenvolvida na etapa seguinte do projeto.

---

## 👥 Equipe e Atribuições

O projeto consolida as contribuições desenvolvidas pelos integrantes da equipe em suas respectivas branches:

- **Carlos** (`pi-carlos`):
  - Modelagem e controle de status dos chalés (`backend/chales.c`).
  - Tela do mapa semanal de ocupação em tabela ASCII com avanço/retrocesso de semanas (`telas/tela_mapa.c`).
- **João** (`pi-joao`):
  - Painel principal do operador da recepção (`telas/tela_recepcao.c`).
  - Máquina de estados para navegação e resumo diário de atendimentos.
- **Rayane** (`pi-rayane`):
  - Módulo de autenticação de funcionários (`backend/auth.c` e `telas/tela_login.c`).
  - Módulo e relatório de auditoria de operações críticas com paginação e filtro por usuário (`backend/auditoria.c` e `telas/tela_auditoria.c`).
- **Enzo** (`pi-enzo`):
  - Integração geral dos módulos em C, estruturação da arquitetura em camadas (`backend/`, `telas/`, `common/`).
  - Módulos de reservas, hóspedes e consumo (`backend/reservas.c`, `backend/hospedes.c`, `backend/consumo.c`, `telas/tela_operacoes.c`).
  - Automação de compilação multiplataforma com `Makefile` e documentação técnica.

> **Arquivos Originais:** Os arquivos de código enviados individualmente nas branches continuam preservados na pasta [`originais/`](originais/) para consulta e avaliação do professor.

---

## 📁 Estrutura do Projeto

```text
Projeto-Integrador/
├── Makefile                     # Compilação e execução automatizada
├── README.md                    # Documentação oficial do projeto
├── .gitignore                   # Arquivos temporários e executáveis ignorados
├── main.c                       # Ponto de entrada do sistema
│
├── backend/                     # REGRAS DE NEGÓCIO, ESTRUTURAS E DADOS EM C
│   ├── auth.c / .h              # Verificação de credenciais e perfis (Rayane)
│   ├── chales.c / .h            # Controle de chalés e status (Carlos)
│   ├── reservas.c / .h          # Gerenciamento de reservas, check-in e check-out
│   ├── hospedes.c / .h          # Cadastro e consulta de hóspedes
│   ├── consumo.c / .h           # Lançamento e cálculo de consumo dos chalés
│   └── auditoria.c / .h         # Armazenamento e log de operações críticas (Rayane)
│
├── telas/                       # INTERFACE COM O USUÁRIO (Telas de Console / Terminal)
│   ├── tela_login.c / .h        # Tela de Login de funcionários (Rayane)
│   ├── tela_recepcao.c / .h     # Painel principal e máquina de estados (João)
│   ├── tela_mapa.c / .h         # Tela do mapa semanal de chalés (Carlos)
│   ├── tela_auditoria.c / .h    # Tela do relatório de auditoria paginado (Rayane)
│   └── tela_operacoes.c / .h    # Subtelas de Check-in, Check-out, Reservas, etc.
│
├── common/                      # UTILITÁRIOS GLOBAIS
│   └── common.c / .h            # Limpeza de tela multiplataforma, leitura segura e datas
│
└── originais/                   # CÓDIGOS ORIGINAIS DAS BRANCHES INDIVIDUAIS
    ├── Gerenciador-chales.c     # Carlos
    ├── auditoriadoadm.c         # Rayane
    ├── paginainicial.c          # Rayane
    └── painelRecep.c            # João
```

---

## 🚀 Como Compilar e Executar

### Opção 1: Usando Make (Recomendado)

No terminal, na pasta raiz do projeto, execute:

```bash
make run
```

Para apenas compilar:
```bash
make
```

Para limpar os arquivos de compilação:
```bash
make clean
```

---

### Opção 2: Compilação Direta via GCC / Clang

Se não possuir o comando `make` instalado:

- **No macOS / Linux:**
  ```bash
  gcc -Wall -Wextra -std=c99 -I. -Icommon -Ibackend -Itelas main.c common/*.c backend/*.c telas/*.c -o sistema_recanto
  ./sistema_recanto
  ```

- **No Windows (MinGW / Prompt de Comando):**
  ```cmd
  gcc -Wall -Wextra -std=c99 -I. -Icommon -Ibackend -Itelas main.c common/*.c backend/*.c telas/*.c -o sistema_recanto.exe
  sistema_recanto.exe
  ```

---

## 🔑 Credenciais para Teste

O sistema conta com três perfis de usuário pré-configurados:

| Usuário | Senha | Perfil | Telas e Acessos Permitidos |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel da Recepção, Mapa do Carlos, Check-in, Check-out, Reservas, Consumo, Hóspedes |
| `ana.governanca` | `123456` | Governança | Gerenciamento de limpeza/manutenção de chalés e Mapa |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria da Rayane e Painel da Recepção |
