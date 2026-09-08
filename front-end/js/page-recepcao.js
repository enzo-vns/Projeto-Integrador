/* ---------------- página: painel da recepção ---------------- */
initShell();

document.getElementById('chip-checkin').textContent = pendentesCheckinList().length;
document.getElementById('chip-checkout').textContent = pendentesCheckoutList().length;
document.getElementById('chip-livres').textContent = state.chales.filter(c=>c.status==='livre').length;
