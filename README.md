# Recanto Bosque — Sistema de Gestão Hoteleira

Projeto Integrador desenvolvido para gestão das operações do hotel/pousada **Recanto Bosque**. O sistema contempla o fluxo completo de atendimento, controle de chalés, governança, check-in, check-out, lançamento de consumo e auditoria administrativa.

---

## 👥 Equipe e Divisão de Módulos

O projeto integra os desenvolvimentos realizados nas diferentes branches da equipe:

- **Enzo** (`pi-enzo`): Desenvolvimento do **Front-end Web** (HTML, CSS e JavaScript puro, sem frameworks pesados).
- **Carlos** (`pi-carlos`): Desenvolvimento do **Mapa de Ocupação** e gerenciamento de status dos chalés em C.
- **João** (`pi-joao`): Desenvolvimento do **Painel da Recepção** e máquina de estados para navegação em C.
- **Rayane** (`pi-rayane`): Desenvolvimento da **Autenticação (Login)** e do módulo de **Auditoria de Operações** em C.

---

## 📁 Estrutura do Projeto

```text
Projeto-Integrador/
├── front-end/               # Interface Web completa
│   ├── html/                # Telas (login, recepção, mapa, auditoria, etc.)
│   ├── css/                 # Folhas de estilo (style.css)
│   ├── js/                  # Estado global (store.js), shell e lógica das páginas
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

## 🚀 Como Executar

### 1. Front-end (Web)

O front-end não requer etapa de build. Para executá-lo:

**Com Python:**
```bash
cd front-end
python3 -m http.server 8000
```
Acesse no navegador: [http://localhost:8000/html/index.html](http://localhost:8000/html/index.html)

**Com Node.js (opcional):**
```bash
cd front-end
npx serve .
```

---

### 2. Back-end (C)

O back-end é compilável via `make` ou compilador C padrão (`gcc` / `clang`):

```bash
cd back-end
make
make run
```

Ou compilação direta:
```bash
cd back-end
gcc -Wall -Wextra -std=c99 -Iinclude src/*.c -o bin/sistema_recanto
./bin/sistema_recanto
```

---

## 🔑 Credenciais para Teste

As mesmas credenciais são aceitas tanto no sistema em C quanto no Front-end:

| Usuário | Senha | Perfil | Telas / Acessos |
|---|---|---|---|
| `marcos.recepcao` | `123456` | Recepção | Painel Geral, Reservas, Check-in/out, Consumo, Mapa |
| `ana.governanca` | `123456` | Governança | Status dos chalés, limpeza, manutenção e Mapa |
| `diretoria.admin` | `123456` | Administrador | Relatório de Auditoria completo, Mapa e Painel |
