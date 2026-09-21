/* ---------------- página: log de auditoria ---------------- */
initShell();

const AUDIT_PAGE_SIZE = 3;
const params = new URLSearchParams(location.search);
const filterUser = params.get('user') || '';
let page = Math.max(0, Number(params.get('page') || '0'));

function filtered(){
  return filterUser
    ? state.auditLog.filter(l => l.usuario.toLowerCase().includes(filterUser.toLowerCase()))
    : state.auditLog;
}

function pageUrl(p, user){
  const usp = new URLSearchParams();
  if(user) usp.set('user', user);
  if(p) usp.set('page', String(p));
  const qs = usp.toString();
  return 'auditoria.html' + (qs ? `?${qs}` : '');
}

const list = filtered();
const totalPages = Math.max(1, Math.ceil(list.length / AUDIT_PAGE_SIZE));
page = Math.min(page, totalPages - 1);
const pageItems = list.slice(page*AUDIT_PAGE_SIZE, page*AUDIT_PAGE_SIZE + AUDIT_PAGE_SIZE);

document.getElementById('audit-count').textContent =
  `${list.length} de ${state.auditLog.length} registro(s) · página ${page+1} de ${totalPages}`;

document.getElementById('audit-tbody').innerHTML = pageItems.length
  ? pageItems.map(l=>`
      <tr>
        <td>${l.hora}</td>
        <td>${l.usuario}</td>
        <td>${l.acao}</td>
        <td>${l.detalhe}</td>
      </tr>`).join('')
  : `<tr><td colspan="4"><div class="empty-state">Nenhum registro encontrado${filterUser ? ` para o usuário "${filterUser}"` : ''}.</div></td></tr>`;

const filterInput = document.getElementById('audit-filter');
filterInput.value = filterUser;
filterInput.addEventListener('change', ()=>{
  location.href = pageUrl(0, filterInput.value.trim());
});

const clearBtn = document.getElementById('audit-clear');
clearBtn.disabled = !filterUser;
clearBtn.addEventListener('click', ()=>{ location.href = 'auditoria.html'; });

const prevBtn = document.getElementById('audit-prev');
const nextBtn = document.getElementById('audit-next');
if(page<=0){ prevBtn.setAttribute('disabled',''); prevBtn.removeAttribute('href'); }
else { prevBtn.href = pageUrl(page-1, filterUser); }
if(page>=totalPages-1){ nextBtn.setAttribute('disabled',''); nextBtn.removeAttribute('href'); }
else { nextBtn.href = pageUrl(page+1, filterUser); }
