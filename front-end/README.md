# Recanto Bosque — Sistema de Gestão

Front-end estático (HTML + CSS + JS puro, sem build) do sistema de gestão do Recanto Bosque: login, painel da recepção, reservas, mapa de ocupação, governança/manutenção e log de auditoria.

## Estrutura

```
front-end/
├── html/   → todas as páginas (uma por tela)
├── css/    → estilos (style.css)
├── js/     → estado da aplicação + lógica de cada página
└── README.md
```

- `html/index.html` é a página de entrada (login).
- Cada página em `html/` carrega `js/store.js` (estado) e `js/shell.js` (sidebar/topbar/autenticação), além do seu próprio `js/page-*.js`.
- O estado (reservas, chalés, hóspedes, log de auditoria etc.) é salvo no `localStorage` do navegador, então ele persiste entre as páginas mesmo sem back-end.

## Como rodar

Este projeto não tem build nem dependências — só precisa ser servido por um servidor HTTP simples (abrir o `index.html` direto com `file://` não funciona bem por causa do `fetch`/`localStorage` em alguns navegadores).

**Opção 1 — Python (já vem instalado na maioria dos sistemas):**

```bash
cd front-end
python3 -m http.server 8000
```

Depois acesse: http://localhost:8000/html/index.html

**Opção 2 — VS Code:**

Instale a extensão "Live Server" e clique em "Go Live" com `html/index.html` aberto.

**Opção 3 — Node:**

```bash
cd front-end
npx serve .
```

## Login de teste

| Usuário | Perfil | Telas visíveis |
|---|---|---|
| `marcos.recepcao` | Recepção | Painel, Reservas, Mapa |
| `ana.governanca` | Governança | Governança & Manutenção |
| `diretoria.admin` | Administrador | Log de Auditoria, Mapa |

Senha: qualquer valor com 4 ou mais caracteres.

## Resetar os dados de teste

Os dados (reservas, chalés, hóspedes, auditoria) ficam salvos no `localStorage` do navegador. Para voltar ao estado inicial, abra o DevTools do navegador (F12) → aba Console → rode:

```js
localStorage.removeItem('rb_state_v1')
```

e recarregue a página.
