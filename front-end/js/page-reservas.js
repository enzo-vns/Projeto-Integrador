/* ---------------- página: reservas ---------------- */
initShell();

function reservaRow(r){
  const disabled = r.status === 'cancelada';
  return `<tr id="reserva-row-${r.id}">
    <td>#${r.id}</td>
    <td>${r.hospede}</td>
    <td>${r.chale}</td>
    <td>${r.checkin}</td>
    <td>${r.checkout}</td>
    <td><span class="status-pill s-${r.status}">${STATUS_LABEL[r.status]}</span></td>
    <td>
      <a class="cell-btn" href="reserva-form.html?id=${r.id}" ${disabled?'disabled':''}>editar</a>
      <button class="cell-btn warn" onclick="cancelReserva(${r.id})" ${disabled?'disabled':''}>cancelar</button>
    </td>
  </tr>`;
}

function renderReservas(){
  document.getElementById('reservas-count').textContent = `${state.reservas.length} reserva(s)`;
  document.getElementById('reservas-tbody').innerHTML =
    state.reservas.map(reservaRow).join('') ||
    `<tr><td colspan="7"><div class="empty-state">Nenhuma reserva cadastrada.</div></td></tr>`;
}

function cancelReserva(id){
  const r = state.reservas.find(x=>x.id===id);
  if(!r) return;
  const ok = confirm(`Cancelar a reserva #${id} de ${r.hospede}? Esta ação será registrada na auditoria.`);
  if(!ok) return;
  r.status = 'cancelada';
  saveState();
  addAuditEntry('CANCELA_RESERVA', `Res #${id} — Motivo: cancelamento manual`);
  renderReservas();
}

renderReservas();
