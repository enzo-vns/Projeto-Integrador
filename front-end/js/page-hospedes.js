/* ---------------- página: buscar / cadastrar hóspede ---------------- */
initShell();

function renderHospedes(term){
  const filtrados = term
    ? state.hospedes.filter(h=>h.nome.toLowerCase().includes(term.toLowerCase()) || h.documento.includes(term))
    : state.hospedes;
  document.getElementById('hospedes-tbody').innerHTML = filtrados.length
    ? filtrados.map(h=>`<tr><td>${h.nome}</td><td>${h.documento}</td><td>${h.telefone}</td></tr>`).join('')
    : `<tr><td colspan="3"><div class="empty-state">Nenhum hóspede encontrado.</div></td></tr>`;
}

document.getElementById('hospede-search').addEventListener('input', (e)=>renderHospedes(e.target.value));

renderHospedes('');
