/* ---------------- página: cadastrar hóspede ---------------- */
initShell();

function salvarHospede(){
  const nome = document.getElementById('f-hosp-nome').value.trim();
  const documento = document.getElementById('f-hosp-doc').value.trim();
  const telefone = document.getElementById('f-hosp-tel').value.trim();
  const errEl = document.getElementById('hospede-error');
  if(!nome || !documento){
    errEl.style.display='block';
    errEl.textContent = 'Informe ao menos nome e documento.';
    return;
  }
  state.hospedes.push({ id: state.nextHospedeId++, nome, documento, telefone });
  saveState();
  addAuditEntry('CADASTRA_HOSPEDE', `${nome} (${documento})`);
  location.href = 'hospedes.html';
}
