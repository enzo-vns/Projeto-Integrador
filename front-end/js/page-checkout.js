/* ---------------- página: realizar check-out & fechamento ---------------- */
initShell();

function renderCheckout(){
  const pendentes = pendentesCheckoutList();
  document.getElementById('checkout-tbody').innerHTML = pendentes.length
    ? pendentes.map(r=>{
        const total = state.consumoLog.filter(c=>c.reservaId===r.id).reduce((s,c)=>s+c.valor,0);
        return `<tr>
          <td>#${r.id}</td><td>${r.hospede}</td><td>${r.chale}</td><td>R$ ${total.toFixed(2)}</td>
          <td><button class="cell-btn" onclick="confirmarCheckout(${r.id})">confirmar check-out</button></td>
        </tr>`;
      }).join('')
    : `<tr><td colspan="5"><div class="empty-state">Nenhum check-out pendente.</div></td></tr>`;
}

function confirmarCheckout(id){
  const r = state.reservas.find(x=>x.id===id);
  if(!r) return;
  const total = state.consumoLog.filter(c=>c.reservaId===r.id).reduce((s,c)=>s+c.valor,0);
  const ok = confirm(`Fechar conta de ${r.hospede} (total R$ ${total.toFixed(2)}) e liberar o chalé ${r.chale} para limpeza?`);
  if(!ok) return;
  r.checkedOut = true;
  const c = state.chales.find(x=>x.id===r.chale);
  if(c) c.status = 'limpeza';
  saveState();
  addAuditEntry('CHECKOUT_REALIZADO', `Reserva #${r.id} — Total R$ ${total.toFixed(2)}`);
  renderCheckout();
}

renderCheckout();
