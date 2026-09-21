/* ---------------- página: lançar consumo / extrato da conta ---------------- */
initShell();

function ativas(){
  return state.reservas.filter(r=>r.checkedIn && !r.checkedOut);
}

function renderConsumo(){
  const lista = ativas();
  const select = document.getElementById('f-consumo-reserva');
  select.innerHTML = lista.length
    ? lista.map(r=>`<option value="${r.id}">#${r.id} — ${r.hospede} (${r.chale})</option>`).join('')
    : `<option value="">nenhum hóspede com check-in ativo</option>`;
  document.getElementById('btn-lancar').disabled = lista.length === 0;

  document.getElementById('consumo-tbody').innerHTML = state.consumoLog.length
    ? state.consumoLog.map(c=>{
        const r = state.reservas.find(x=>x.id===c.reservaId);
        return `<tr><td>#${c.reservaId}</td><td>${r?r.hospede:'—'}</td><td>${c.item}</td><td>R$ ${c.valor.toFixed(2)}</td></tr>`;
      }).join('')
    : `<tr><td colspan="4"><div class="empty-state">Nenhum lançamento ainda.</div></td></tr>`;
}

document.getElementById('btn-lancar').addEventListener('click', ()=>{
  const reservaId = Number(document.getElementById('f-consumo-reserva').value);
  const item = document.getElementById('f-consumo-item').value.trim();
  const valor = parseFloat(document.getElementById('f-consumo-valor').value);
  const errEl = document.getElementById('consumo-error');
  if(!reservaId || !item || isNaN(valor) || valor <= 0){
    errEl.style.display='block';
    errEl.textContent = 'Selecione o hóspede e informe item e valor válidos.';
    return;
  }
  errEl.style.display='none';
  state.consumoLog.push({ id: state.nextConsumoId++, reservaId, item, valor });
  saveState();
  const r = state.reservas.find(x=>x.id===reservaId);
  addAuditEntry('LANCA_CONSUMO', `Reserva #${reservaId} (${r?r.hospede:''}) — ${item}: R$ ${valor.toFixed(2)}`);
  document.getElementById('f-consumo-item').value = '';
  document.getElementById('f-consumo-valor').value = '';
  renderConsumo();
});

renderConsumo();
