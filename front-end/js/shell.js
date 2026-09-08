/* ---------------- shell: autenticação, sidebar e topbar (comum a todas as páginas internas) ---------------- */

function requireAuth(){
  if(!state.currentUser){
    location.href = 'index.html';
    return null;
  }
  return state.currentUser;
}

function applyRolePermissions(){
  const role = state.currentUser.role;
  document.querySelectorAll('.nav-item[data-screen]').forEach(el=>{
    const screen = el.dataset.screen;
    let allowed = true;
    if(role === 'recepcao') allowed = ['recepcao','reservas','mapa'].includes(screen);
    if(role === 'governanca') allowed = (screen==='governanca');
    if(role === 'admin') allowed = ['auditoria','mapa'].includes(screen);
    el.style.display = allowed ? 'flex' : 'none';
  });
}

function doLogoff(){
  if(state.currentUser) addAuditEntry('LOGOFF', `Sessão encerrada por ${state.currentUser.name}`);
  state.currentUser = null;
  saveState();
  location.href = 'index.html';
}

function roleHome(){
  return state.currentUser ? (ROLE_HOME[state.currentUser.role] || 'recepcao.html') : 'index.html';
}

/* Chama no topo de cada página protegida. Redireciona para o login se não houver sessão,
   e preenche perfil/operador no cabeçalho. */
function initShell(){
  const user = requireAuth();
  if(!user) return null;
  const profileTag = document.getElementById('profile-tag');
  if(profileTag) profileTag.textContent = user.roleLabel;
  const metaOperator = document.getElementById('meta-operator');
  if(metaOperator) metaOperator.textContent = `Operador: ${user.name} (${user.roleLabel})`;
  applyRolePermissions();
  return user;
}
