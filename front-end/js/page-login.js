/* ---------------- página: login ---------------- */

if(state.currentUser){
  location.href = ROLE_HOME[state.currentUser.role] || 'recepcao.html';
}

function clearLoginErrors(){
  ['user-error','pass-error'].forEach(id=>{
    const el = document.getElementById(id);
    el.style.display='none'; el.textContent='';
  });
  document.getElementById('login-user').classList.remove('field-error');
  document.getElementById('login-pass').classList.remove('field-error');
}

function doLogin(){
  clearLoginErrors();
  const u = document.getElementById('login-user').value.trim();
  const p = document.getElementById('login-pass').value;
  const status = document.getElementById('login-status');
  status.className = 'login-status';
  status.textContent = '';

  let hasError = false;
  if(!u){
    document.getElementById('login-user').classList.add('field-error');
    const el = document.getElementById('user-error'); el.style.display='block'; el.textContent='Informe o usuário.';
    hasError = true;
  } else if(!USERS[u]){
    document.getElementById('login-user').classList.add('field-error');
    const el = document.getElementById('user-error'); el.style.display='block'; el.textContent='Usuário não encontrado.';
    hasError = true;
  }
  if(!p || p.length < 4){
    document.getElementById('login-pass').classList.add('field-error');
    const el = document.getElementById('pass-error'); el.style.display='block'; el.textContent='Senha deve ter ao menos 4 caracteres.';
    hasError = true;
  }
  if(hasError){
    status.classList.add('err');
    status.textContent = 'Não foi possível autenticar. Corrija os campos acima.';
    return;
  }

  const user = USERS[u];
  status.textContent = `Login realizado com sucesso! Perfil detectado: ${user.roleLabel.toUpperCase()}`;
  state.currentUser = user;
  saveState();
  setTimeout(()=>{
    addAuditEntry('LOGIN', `Acesso ao painel de ${user.roleLabel}`);
    location.href = ROLE_HOME[user.role] || 'recepcao.html';
  }, 450);
}
