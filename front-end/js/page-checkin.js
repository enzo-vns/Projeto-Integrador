/* ---------------- página: realizar check-in ---------------- */
initShell();

function renderCheckin(){
  const pendentes = pendentesCheckinList();
  document.getElementById('checkin-tbody').innerHTML = pendentes.length
    ? pendentes.map(r=>`
      <tr>
        <td>#${r.id}</td><td>${r.hospede}</td><td>${r.chale}</td><td>${r.checkin}</td>
        <td><button class="cell-btn" onclick="confirmarCheckin(${r.id})">confirmar check-in</button></td>
      </tr>`).join('')
    : `<tr><td colspan="5"><div class="empty-state">Nenhum check-in pendente.</div></td></tr>`;
}

function confirmarCheckin(id){
  const r = state.reservas.find(x=>x.id===id);
  if(!r) return;
  const ok = confirm(`Confirmar check-in de ${r.hospede} no chalé ${r.chale}?`);
  if(!ok) return;
  r.checkedIn = true;
  const c = state.chales.find(x=>x.id===r.chale);
  if(c) c.status = 'ocupado';
  saveState();
  addAuditEntry('CHECKIN_REALIZADO', `Reserva #${r.id} (Hóspede: ${r.hospede})`);
  renderCheckin();
}

renderCheckin();
