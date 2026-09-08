/* ---------------- página: nova / editar reserva ---------------- */
initShell();

const params = new URLSearchParams(location.search);
const editingId = params.has('id') ? Number(params.get('id')) : null;
const editing = editingId ? state.reservas.find(r=>r.id===editingId) : null;

document.getElementById('f-chale').innerHTML = state.chales
  .map(c=>`<option value="${c.id}">${c.id} — ${c.tipo}</option>`).join('');

if(editing){
  document.getElementById('form-title-page').textContent = `Editar reserva #${editing.id}`;
  document.getElementById('form-title').textContent = `Editar reserva #${editing.id}`;
  document.getElementById('f-hospede').value = editing.hospede;
  document.getElementById('f-chale').value = editing.chale;
  document.getElementById('f-checkin').value = editing.checkin;
  document.getElementById('f-checkout').value = editing.checkout;
}

function saveReserva(){
  const hospede = document.getElementById('f-hospede').value.trim();
  const chale = document.getElementById('f-chale').value;
  const checkin = document.getElementById('f-checkin').value;
  const checkout = document.getElementById('f-checkout').value;
  const errEl = document.getElementById('form-error');

  if(!hospede || !checkin || !checkout){
    errEl.style.display='block';
    errEl.textContent = 'Preencha hóspede, check-in e check-out.';
    return;
  }
  if(checkout <= checkin){
    errEl.style.display='block';
    errEl.textContent = 'Check-out deve ser depois do check-in.';
    return;
  }

  if(editing){
    Object.assign(editing, { hospede, chale, checkin, checkout });
    saveState();
    addAuditEntry('EDITA_RESERVA', `Reserva #${editing.id} atualizada (${hospede}, ${chale})`);
  } else {
    const novo = { id: state.nextReservaId++, hospede, chale, checkin, checkout, status:'confirmada', checkedIn:false, checkedOut:false };
    state.reservas.push(novo);
    saveState();
    addAuditEntry('NOVA_RESERVA', `Reserva #${novo.id} criada (${hospede}, ${chale})`);
  }
  location.href = 'reservas.html';
}
