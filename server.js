const http = require('http');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');

const PORT = process.env.PORT || 3000;
const FRONT_END_DIR = path.join(__dirname, 'front-end');
const DB_FILE = path.join(__dirname, 'data', 'database.json');

const USERS = {
  'marcos.recepcao': { name: 'Marcos', role: 'recepcao', roleLabel: 'Recepção' },
  'ana.governanca':  { name: 'Ana',    role: 'governanca', roleLabel: 'Governança' },
  'diretoria.admin': { name: 'Diretoria', role: 'admin', roleLabel: 'Administrador' }
};

const MIME_TYPES = {
  '.html': 'text/html; charset=UTF-8',
  '.css':  'text/css; charset=UTF-8',
  '.js':   'application/javascript; charset=UTF-8',
  '.json': 'application/json; charset=UTF-8',
  '.png':  'image/png',
  '.jpg':  'image/jpeg',
  '.svg':  'image/svg+xml',
  '.ico':  'image/x-icon'
};

// Carrega ou inicializa o banco de dados JSON
function carregarBanco() {
  try {
    if (fs.existsSync(DB_FILE)) {
      const conteudo = fs.readFileSync(DB_FILE, 'utf8');
      return JSON.parse(conteudo);
    }
  } catch (err) {
    console.error('⚠️ Erro ao ler database.json, usando padrão:', err.message);
  }
  return {
    chales: [
      { id: 'CH-01', tipo: 'Luxo', status: 'ocupado' },
      { id: 'CH-02', tipo: 'Família', status: 'limpeza' },
      { id: 'CH-03', tipo: 'Standard', status: 'livre' },
      { id: 'CH-04', tipo: 'Standard', status: 'manutencao' }
    ],
    reservas: [
      { id: 104, hospede: 'Carlos Andrade', chale: 'CH-01', checkin: '2026-09-08', checkout: '2026-09-10', status: 'confirmada', checkedIn: false, checkedOut: false },
      { id: 105, hospede: 'Beatriz Lima', chale: 'CH-02', checkin: '2026-09-10', checkout: '2026-09-13', status: 'confirmada', checkedIn: false, checkedOut: false },
      { id: 98,  hospede: 'Renato Souza', chale: 'CH-03', checkin: '2026-09-05', checkout: '2026-09-07', status: 'cancelada',  checkedIn: false, checkedOut: false }
    ],
    nextReservaId: 106,
    hospedes: [
      { id: 1, nome: 'Carlos Andrade', documento: '321.654.987-00', telefone: '(11) 98888-1122' },
      { id: 2, nome: 'Beatriz Lima',   documento: '112.334.556-70', telefone: '(11) 97777-3344' },
      { id: 3, nome: 'Renato Souza',   documento: '998.877.665-40', telefone: '(11) 96666-5566' }
    ],
    nextHospedeId: 4,
    consumoLog: [],
    nextConsumoId: 1,
    auditLog: [
      { hora: '20/09/2026 18:36', usuario: 'ana.gov', acao: 'STATUS_CHALE', detalhe: 'CH-02 alterado para LIMPEZA' },
      { hora: '20/09/2026 21:00', usuario: 'marcos.rec', acao: 'CHECKIN_REALIZADO', detalhe: 'Reserva #87 (Hospede: Carlos)' },
      { hora: '20/09/2026 21:43', usuario: 'marcos.rec', acao: 'CANCELA_RESERVA', detalhe: 'Reserva #97 - Motivo: Desistencia' }
    ]
  };
}

let db = carregarBanco();

function salvarBanco() {
  try {
    const dir = path.dirname(DB_FILE);
    if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
    fs.writeFileSync(DB_FILE, JSON.stringify(db, null, 2), 'utf8');
  } catch (err) {
    console.error('⚠️ Erro ao salvar database.json:', err.message);
  }
}

function registrarAuditoria(usuario, acao, detalhe) {
  const agora = new Date();
  const pad = (n) => String(n).padStart(2, '0');
  const dataStr = `${pad(agora.getDate())}/${pad(agora.getMonth() + 1)}/${agora.getFullYear()} ${pad(agora.getHours())}:${pad(agora.getMinutes())}`;

  if (!db.auditLog) db.auditLog = [];
  db.auditLog.unshift({ hora: dataStr, usuario: usuario || 'sistema', acao, detalhe });
  salvarBanco();
  console.log(`📝 [AUDITORIA] ${usuario} -> ${acao}: ${detalhe}`);
}

function parseJsonBody(req) {
  return new Promise((resolve, reject) => {
    let body = '';
    req.on('data', chunk => body += chunk);
    req.on('end', () => {
      try {
        resolve(body ? JSON.parse(body) : {});
      } catch (err) {
        reject(err);
      }
    });
    req.on('error', reject);
  });
}

function sendJson(res, statusCode, data) {
  res.writeHead(statusCode, {
    'Content-Type': 'application/json; charset=UTF-8',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type'
  });
  res.end(JSON.stringify(data));
}

// Servidor HTTP
const server = http.createServer(async (req, res) => {
  // CORS preflight
  if (req.method === 'OPTIONS') {
    res.writeHead(204, {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type'
    });
    return res.end();
  }

  const parsedUrl = new URL(req.url, `http://${req.headers.host}`);
  const pathname = parsedUrl.pathname;

  // ================= ROTAS DE API (BACK-END) =================
  if (pathname.startsWith('/api/')) {
    console.log(`🌐 [API] ${req.method} ${pathname}`);

    // Estado completo
    if (pathname === '/api/state' && req.method === 'GET') {
      return sendJson(res, 200, db);
    }
    if (pathname === '/api/state' && req.method === 'POST') {
      try {
        const body = await parseJsonBody(req);
        if (body.chales) db.chales = body.chales;
        if (body.reservas) db.reservas = body.reservas;
        if (body.hospedes) db.hospedes = body.hospedes;
        if (body.consumoLog) db.consumoLog = body.consumoLog;
        if (body.auditLog) db.auditLog = body.auditLog;
        if (body.nextReservaId) db.nextReservaId = body.nextReservaId;
        if (body.nextHospedeId) db.nextHospedeId = body.nextHospedeId;
        if (body.nextConsumoId) db.nextConsumoId = body.nextConsumoId;
        salvarBanco();
        return sendJson(res, 200, { success: true, state: db });
      } catch (err) {
        return sendJson(res, 400, { error: 'JSON inválido' });
      }
    }

    // Login
    if (pathname === '/api/login' && req.method === 'POST') {
      try {
        const { usuario, senha } = await parseJsonBody(req);
        const user = USERS[usuario];
        if (user && senha && senha.length >= 4) {
          registrarAuditoria(usuario, 'LOGIN', `Acesso ao painel de ${user.roleLabel}`);
          return sendJson(res, 200, { success: true, user });
        }
        return sendJson(res, 401, { error: 'Credenciais inválidas' });
      } catch (err) {
        return sendJson(res, 400, { error: 'Erro na requisição' });
      }
    }

    // Chalés
    if (pathname === '/api/chales' && req.method === 'GET') {
      return sendJson(res, 200, db.chales);
    }

    // Reservas
    if (pathname === '/api/reservas' && req.method === 'GET') {
      return sendJson(res, 200, db.reservas);
    }

    // Hóspedes
    if (pathname === '/api/hospedes' && req.method === 'GET') {
      return sendJson(res, 200, db.hospedes);
    }

    // Auditoria
    if (pathname === '/api/auditoria' && req.method === 'GET') {
      return sendJson(res, 200, db.auditLog);
    }
    if (pathname === '/api/auditoria' && req.method === 'POST') {
      try {
        const { usuario, acao, detalhe } = await parseJsonBody(req);
        registrarAuditoria(usuario, acao, detalhe);
        return sendJson(res, 201, { success: true });
      } catch (err) {
        return sendJson(res, 400, { error: 'Erro ao registrar auditoria' });
      }
    }

    // Check-in
    if (pathname === '/api/checkin' && req.method === 'POST') {
      try {
        const { reservaId, usuario } = await parseJsonBody(req);
        const reserva = db.reservas.find(r => r.id === Number(reservaId));
        if (!reserva) return sendJson(res, 404, { error: 'Reserva não encontrada' });

        reserva.checkedIn = true;
        const chale = db.chales.find(c => c.id === reserva.chale);
        if (chale) chale.status = 'ocupado';

        registrarAuditoria(usuario, 'CHECKIN_REALIZADO', `Reserva #${reserva.id} (Hóspede: ${reserva.hospede})`);
        salvarBanco();
        return sendJson(res, 200, { success: true, reserva });
      } catch (err) {
        return sendJson(res, 400, { error: 'Erro ao realizar check-in' });
      }
    }

    // Check-out
    if (pathname === '/api/checkout' && req.method === 'POST') {
      try {
        const { reservaId, usuario } = await parseJsonBody(req);
        const reserva = db.reservas.find(r => r.id === Number(reservaId));
        if (!reserva) return sendJson(res, 404, { error: 'Reserva não encontrada' });

        reserva.checkedOut = true;
        const chale = db.chales.find(c => c.id === reserva.chale);
        if (chale) chale.status = 'limpeza';

        registrarAuditoria(usuario, 'CHECKOUT_REALIZADO', `Reserva #${reserva.id} finalizada. ${reserva.chale} para LIMPEZA`);
        salvarBanco();
        return sendJson(res, 200, { success: true, reserva });
      } catch (err) {
        return sendJson(res, 400, { error: 'Erro ao realizar check-out' });
      }
    }

    // Rota API não encontrada
    return sendJson(res, 404, { error: 'Endpoint de API não encontrado' });
  }

  // ================= ARQUIVOS ESTÁTICOS (FRONT-END) =================
  let filePath;

  // Redireciona a raiz para /html/index.html
  if (pathname === '/' || pathname === '/index.html') {
    res.writeHead(302, { Location: '/html/index.html' });
    return res.end();
  }

  // Resolve arquivos dentro de front-end
  filePath = path.join(FRONT_END_DIR, pathname);

  // Impede navegação fora da pasta front-end
  if (!filePath.startsWith(FRONT_END_DIR)) {
    res.writeHead(403, { 'Content-Type': 'text/plain' });
    return res.end('403 - Acesso Negado');
  }

  fs.stat(filePath, (err, stats) => {
    if (err || !stats.isFile()) {
      res.writeHead(404, { 'Content-Type': 'text/html; charset=UTF-8' });
      return res.end(`
        <body style="font-family:sans-serif; text-align:center; padding-top:60px;">
          <h2>404 — Página não encontrada</h2>
          <p><a href="/html/index.html">Ir para a página inicial do Recanto Bosque</a></p>
        </body>
      `);
    }

    const ext = path.extname(filePath).toLowerCase();
    const contentType = MIME_TYPES[ext] || 'application/octet-stream';

    res.writeHead(200, { 'Content-Type': contentType });
    fs.createReadStream(filePath).pipe(res);
  });
});

server.listen(PORT, () => {
  const url = `http://localhost:${PORT}`;
  console.log('\n============================================================');
  console.log('   🏨 RECANTO BOSQUE — SISTEMA DE GESTÃO INTEGRADO          ');
  console.log('============================================================');
  console.log(` ✅ Servidor Back-end & Front-end ATIVO em: ${url}`);
  console.log(` 🌐 Acesso direto: ${url}/html/index.html`);
  console.log(` 💾 Dados persistidos em: ${path.relative(__dirname, DB_FILE)}`);
  console.log('------------------------------------------------------------');
  console.log(' Credenciais de teste:');
  console.log('  • marcos.recepcao  (Recepção)     — senha: qualquer com 4+ dígitos');
  console.log('  • ana.governanca   (Governança)   — senha: qualquer com 4+ dígitos');
  console.log('  • diretoria.admin  (Administrador)— senha: qualquer com 4+ dígitos');
  console.log('------------------------------------------------------------');
  console.log(' Pressione Ctrl + C no terminal para parar o servidor.');
  console.log('============================================================\n');

  // Abre automaticamente no navegador do usuário
  const comandoAbrir = process.platform === 'darwin' ? `open "${url}"` :
                       process.platform === 'win32'  ? `start "${url}"` :
                       `xdg-open "${url}"`;

  exec(comandoAbrir, (err) => {
    if (err) {
      console.log(`👉 Abra no navegador: ${url}`);
    } else {
      console.log('🚀 Navegador aberto automaticamente!\n');
    }
  });
});
