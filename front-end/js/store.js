/* ---------------- estado compartilhado (persistido em localStorage) ---------------- */
const STORAGE_KEY = 'rb_state_v1';

const USERS = {
  'marcos.recepcao': { name:'Marcos', role:'recepcao', roleLabel:'Recepção' },
  'ana.governanca':  { name:'Ana',    role:'governanca', roleLabel:'Governança' },
  'diretoria.admin': { name:'Diretoria', role:'admin', roleLabel:'Administrador' },
};

const STATUS_LABEL = {
  livre:'Livre', reservado:'Reservado', ocupado:'Ocupado', limpeza:'Limpeza', manutencao:'Manutenção',
  confirmada:'Confirmada', cancelada:'Cancelada'
};

const ROLE_HOME = { recepcao:'recepcao.html', governanca:'governanca.html', admin:'auditoria.html' };

function seedState(){
  return {
    currentUser: null,
    chales: [
      { id:'CH-01', tipo:'Luxo',     status:'ocupado' },
      { id:'CH-02', tipo:'Família',  status:'limpeza' },
      { id:'CH-03', tipo:'Standard', status:'livre' },
      { id:'CH-04', tipo:'Standard', status:'manutencao' },
    ],
    reservas: [
      { id:104, hospede:'Carlos Andrade', chale:'CH-01', checkin:'2026-09-08', checkout:'2026-09-10', status:'confirmada', checkedIn:false, checkedOut:false },
      { id:105, hospede:'Beatriz Lima',   chale:'CH-02', checkin:'2026-09-10', checkout:'2026-09-13', status:'confirmada', checkedIn:false, checkedOut:false },
      { id:98,  hospede:'Renato Souza',   chale:'CH-03', checkin:'2026-09-05', checkout:'2026-09-07', status:'cancelada', checkedIn:false, checkedOut:false },
    ],
    nextReservaId: 106,
    hospedes: [
      { id:1, nome:'Carlos Andrade', documento:'321.654.987-00', telefone:'(11) 98888-1122' },
      { id:2, nome:'Beatriz Lima',   documento:'112.334.556-70', telefone:'(11) 97777-3344' },
      { id:3, nome:'Renato Souza',   documento:'998.877.665-40', telefone:'(11) 96666-5566' },
    ],
    nextHospedeId: 4,
    consumoLog: [],
    nextConsumoId: 1,
    lastGovChange: null,
    auditLog: [
      { hora:'08/09/2026 08:30', usuario:'ana.gov',    acao:'STATUS_CHALE',      detalhe:'CH-02 alterado para LIMPEZA' },
      { hora:'08/09/2026 09:12', usuario:'marcos.rec', acao:'CHECKIN_REALIZADO', detalhe:'Reserva #104 (Hóspede: Carlos)' },
      { hora:'08/09/2026 09:15', usuario:'marcos.rec', acao:'CANCELA_RESERVA',   detalhe:'Res #98 — Motivo: Desistência' },
      { hora:'07/09/2026 19:04', usuario:'diretoria',  acao:'LOGIN',             detalhe:'Acesso ao painel administrativo' },
      { hora:'07/09/2026 17:40', usuario:'ana.gov',    acao:'STATUS_CHALE',      detalhe:'CH-04 alterado para MANUTENÇÃO' },
    ],
  };
}

function loadState(){
  try{
    const raw = localStorage.getItem(STORAGE_KEY);
    if(raw) return JSON.parse(raw);
  } catch(e){ /* localStorage indisponível ou dado corrompido: recomeça do zero */ }
  return seedState();
}

let state = loadState();

function saveState(){
  localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
}

function nowStr(){
  return '08/09/2026 ' + new Date().toTimeString().slice(0,5);
}

function addAuditEntry(acao, detalhe){
  const usuario = state.currentUser ? state.currentUser.name.toLowerCase() : 'sistema';
  state.auditLog.unshift({ hora: nowStr(), usuario, acao, detalhe });
  saveState();
}

function pendentesCheckinList(){
  return state.reservas.filter(r=>r.status==='confirmada' && !r.checkedIn);
}
function pendentesCheckoutList(){
  return state.reservas.filter(r=>r.checkedIn && !r.checkedOut);
}
