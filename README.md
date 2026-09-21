# Recanto Bosque — Sistema de Gestão Hoteleira

Projeto Integrador desenvolvido para a gestão completa das operações do hotel/pousada **Recanto Bosque**. O projeto integra a arquitetura de **Front-end** e **Back-end**, contemplando tanto uma **interface visual (Web)** quanto uma **versão completa em C para terminal**.

---

## 👥 Equipe e Telas Desenvolvidas

As telas do sistema foram modeladas com base nas contribuições dos integrantes da equipe:

- **Rayane** (`pi-rayane`): Desenvolveu a **Tela de Autenticação (Login)** e a tela do **Relatório de Auditoria** de operações com paginação e busca.
- **João** (`pi-joao`): Desenvolveu a **Tela do Painel da Recepção**, resumo do dia (check-ins, check-outs, chalés livres) e a máquina de estados de navegação.
- **Carlos** (`pi-carlos`): Desenvolveu a **Tela do Mapa de Ocupação** semanal dos chalés (CH-01 a CH-04) com navegação de semanas e controle de status.
- **Enzo** (`pi-enzo`): Desenvolveu a **Interface Visual (Front-end Web)** que dá vida a todas essas telas no navegador, o servidor de integração de dados e a arquitetura geral do projeto.

> **Histórico:** Os códigos originais desenvolvidos em C nas branches individuais estão preservados na pasta [`originais/`](originais/).

---

## 📁 Estrutura do Projeto

```text
Projeto-Integrador/
├── server.js                # Servidor Back-end com API REST (Node.js)
├── server.py                # Servidor alternativo em Python
├── package.json             # Scripts de inicialização (npm start)
├── iniciar.sh               # Script de 1 clique para iniciar no terminal
├── data/
│   └── database.json        # Banco de dados persistente compartilhado
│
├── front-end/               # 🌐 INTERFACE VISUAL (Front-end Web)
│   ├── index.html           # Redirecionador automático para o login
│   ├── html/                # Telas (login, recepção, mapa, auditoria, etc.)
│   ├── css/                 # Estilos visuais (style.css)
│   └── js/                  # Lógica das telas (shell.js, store.js, etc.)
│
├── backend/                 # ⚙️ BACK-END EM C (Lógica, dados e regras)
│   ├── auth.c / .h          # Autenticação e perfis de acesso
│   ├── chales.c / .h        # Chalés e controle de ocupação (Carlos)
│   ├── reservas.c / .h      # Gestão de reservas, check-in e check-out
│   ├── hospedes.c / .h      # Cadastro de hóspedes
│   ├── consumo.c / .h       # Lançamento de consumo
│   └── auditoria.c / .h     # Armazenamento de logs (Rayane)
│
├── telas/                   # 🖥️ TELAS EM C (Front-end de console)
│   ├── tela_login.c / .h    # Tela de login em C (Rayane)
│   ├── tela_recepcao.c / .h # Painel de recepção em C (João)
│   ├── tela_mapa.c / .h     # Mapa semanal em C (Carlos)
│   ├── tela_auditoria.c / .h# Relatório de auditoria em C (Rayane)
│   └── tela_operacoes.c / .h# Operações do hotel em C
│
├── common/                  # Utilitários compartilhados
├── main.c                   # Ponto de entrada da versão em C
├── Makefile                 # Compilação da versão em C
└── README.md
```

---

## 🚀 Como Executar o Front-end Visual (Recomendado)

Para ver e usar o sistema com a **interface visual completa** no seu navegador, basta abrir o terminal na pasta do projeto e rodar **um único comando**:

```bash
npm start
```

*(Ou execute: `./iniciar.sh` ou `python3 server.py`)*

### O que acontece:
1. O servidor inicia na porta `3000`.
2. **O seu navegador abre automaticamente** na tela de Login do Recanto Bosque!
3. Todas as telas (Painel do João, Mapa do Carlos, Login e Auditoria da Rayane) funcionam de forma interativa e visual.
4. As alterações ficam salvas no arquivo `data/database.json`.

---

## 💻 Como Executar a Versão em C (Terminal / Console)

Se você ou o professor quiserem rodar a versão do sistema direto no terminal em C:

```bash
make run
```

Para compilar sem rodar:
```bash
make
```

Para limpar arquivos compilados:
```bash
make clean
```

---

## 🔑 Credenciais para Teste

As mesmas credenciais funcionam tanto no Front-end visual quanto na versão em C:

| Usuário | Senha | Perfil | Módulos e Telas |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel Geral, Reservas, Check-in/out, Consumo, Mapa |
| `ana.governanca` | `123456` | Governança | Status dos chalés, limpeza, manutenção e Mapa |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria completo, Mapa e Painel |
