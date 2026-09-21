# Recanto Bosque — Sistema de Gestão Hoteleira

Projeto Integrador desenvolvido para gestão das operações do hotel/pousada **Recanto Bosque**. O sistema contempla o fluxo completo de atendimento, controle de chalés, governança, check-in, check-out, lançamento de consumo e auditoria administrativa.

---

## 👥 Equipe e Divisão de Módulos

O projeto integra os desenvolvimentos realizados nas diferentes branches da equipe:

- **Enzo** (`pi-enzo`): Desenvolvimento do **Front-end Web** (HTML, CSS e JavaScript puro) e servidor de integração API.
- **Carlos** (`pi-carlos`): Desenvolvimento do **Mapa de Ocupação** e gerenciamento de status dos chalés em C.
- **João** (`pi-joao`): Desenvolvimento do **Painel da Recepção** e máquina de estados para navegação em C.
- **Rayane** (`pi-rayane`): Desenvolvimento da **Autenticação (Login)** e do módulo de **Auditoria de Operações** em C.

---

## 📁 Estrutura do Projeto

```text
Projeto-Integrador/
├── server.js                # Servidor HTTP & Back-end API REST unificado
├── package.json             # Scripts de inicialização (npm start)
├── iniciar.sh               # Script de execução rápida para terminal
├── data/
│   └── database.json        # Banco de dados persistente compartilhado
│
├── front-end/               # Interface Web completa
│   ├── index.html           # Redirecionador automático para tela de login
│   ├── html/                # Telas (login, recepção, mapa, auditoria, etc.)
│   ├── css/                 # Folhas de estilo (style.css)
│   ├── js/                  # Estado sincronizado com a API (store.js) e páginas
│   └── README.md            # Documentação específica do Front-end
│
├── back-end/                # Sistema unificado em C (Console / Terminal)
│   ├── include/             # Headers (.h)
│   ├── src/                 # Código-fonte unificado (.c)
│   ├── originais/           # Códigos originais preservados das branches
│   ├── Makefile             # Compilação automatizada
│   └── README.md            # Documentação específica do Back-end
│
├── .gitignore
└── README.md
```

---

## 🚀 Como Executar o Sistema Integrado (Front-end + Back-end)

O jeito mais fácil e completo de rodar o sistema inteiro conectado (servidor back-end servindo a interface web e processando as requisições com persistência em disco):

No terminal, na pasta raiz do projeto:

```bash
npm start
```
*(ou `node server.js` ou `./iniciar.sh`)*

Isso irá:
1. Iniciar o servidor HTTP e a API Back-end em `http://localhost:3000`.
2. Abrir automaticamente a aplicação no seu navegador padrão.
3. Conectar a interface web à API REST com logs de operações em tempo real no terminal.
4. Salvar todas as alterações (check-in, check-out, novas reservas, logs de auditoria) no arquivo `data/database.json`.

---

## 💻 Como Executar o Back-end em C (Terminal / Console)

Para rodar a versão de console em C desenvolvida pela equipe:

```bash
cd back-end
make run
```

Ou compilação direta via GCC:
```bash
cd back-end
gcc -Wall -Wextra -std=c99 -Iinclude src/*.c -o bin/sistema_recanto
./bin/sistema_recanto
```

---

## 🔑 Credenciais para Teste

As mesmas credenciais são aceitas tanto na aplicação web quanto no sistema em C:

| Usuário | Senha | Perfil | Telas / Acessos |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel Geral, Reservas, Check-in/out, Consumo, Mapa |
| `ana.governanca` | `123456` | Governança | Status dos chalés, limpeza, manutenção e Mapa |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria completo, Mapa e Painel |
