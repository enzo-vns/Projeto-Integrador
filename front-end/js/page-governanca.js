/* ---------------- página: governança & manutenção ---------------- */
initShell();

function govRow(c,i){
  let action = `<button class="cell-btn" disabled>bloqueado</button>`;
  if(c.status==='limpeza') action = `<button class="cell-btn" onclick="updateStatus(${i},'livre')">liberar para disponível</button>`;
  if(c.status==='livre') action = `<button class="cell-btn" onclick="updateStatus(${i},'manutencao')">colocar em manutenção</button>`;
  if(c.status==='manutencao') action = `<button class="cell-btn" onclick="updateStatus(${i},'livre')">concluir manutenção</button>`;
  return `<tr id="gov-row-${i}">
    <td>${String(i+1).padStart(2,'0')}</td>
    <td>${c.id}</td>
    <td>${c.tipo}</td>
    <td><span class="status-pill s-${c.status}">${STATUS_LABEL[c.status]}</span></td>
    <td>${action}</td>
  </tr>`;
}

function renderUndoBanner(){
  const slot = document.getElementById('undo-banner-slot');
  if(!state.lastGovChange){ slot.innerHTML=''; return; }
  const { previousStatus, newStatus, chaleId } = state.lastGovChange;
  slot.innerHTML = `
    <div class="undo-banner">
      <span>Última ação: ${chaleId} alterado de <b>${STATUS_LABEL[previousStatus]}</b> para <b>${STATUS_LABEL[newStatus]}</b>.</span>
      <button class="mini-btn" onclick="undoLastGovChange()">desfazer última ação</button>
    </div>`;
}

function renderGovernanca(){
  renderUndoBanner();
  document.getElementById('gov-tbody').innerHTML = state.chales.map((c,i)=>govRow(c,i)).join('');
}

function updateStatus(i, newStatus){
  const c = state.chales[i];
  const ok = confirm(`Alterar ${c.id} de "${STATUS_LABEL[c.status]}" para "${STATUS_LABEL[newStatus]}"? Esta ação fica registrada na auditoria.`);
  if(!ok) return;
  const previousStatus = c.status;
  c.status = newStatus;
  state.lastGovChange = { index: i, previousStatus, newStatus, chaleId: c.id };
  saveState();
  addAuditEntry('STATUS_CHALE', `${c.id} alterado para ${STATUS_LABEL[newStatus].toUpperCase()}`);
  renderGovernanca();
}

function undoLastGovChange(){
  if(!state.lastGovChange) return;
  const { index, previousStatus, newStatus, chaleId } = state.lastGovChange;
  state.chales[index].status = previousStatus;
  state.lastGovChange = null;
  saveState();
  addAuditEntry('REVERTE_STATUS', `${chaleId} revertido de ${STATUS_LABEL[newStatus].toUpperCase()} para ${STATUS_LABEL[previousStatus].toUpperCase()}`);
  renderGovernanca();
}

renderGovernanca();
