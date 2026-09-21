/* ---------------- página: mapa de ocupação ---------------- */
initShell();

const mapaBaseDate = new Date(2026, 8, 8); // Ter 08/09/2026
const diaSemanaAbrev = ['Dom','Seg','Ter','Qua','Qui','Sex','Sáb'];

function buildMapaSemana(offset){
  const statuses = ['livre','reservado','ocupado','limpeza','manutencao'];
  const dias = [];
  for(let d=0; d<5; d++){
    const dt = new Date(mapaBaseDate);
    dt.setDate(dt.getDate() + offset*7 + d);
    dias.push(dt);
  }
  const rows = state.chales.map((c,ci)=>({
    id: c.id,
    tipo: c.tipo,
    dias: dias.map((dt,di)=> statuses[Math.abs(ci*7 + di*3 + offset*5) % statuses.length])
  }));
  return { rows, dias };
}

const weekOffset = Number(new URLSearchParams(location.search).get('week') || '0');
const fmt = dt => `${String(dt.getDate()).padStart(2,'0')}/${String(dt.getMonth()+1).padStart(2,'0')}`;
const fmtFull = dt => `${fmt(dt)}/${dt.getFullYear()}`;

const { rows, dias } = buildMapaSemana(weekOffset);
const primeiro = dias[0], ultimo = dias[dias.length-1];

document.getElementById('mapa-range').textContent =
  `Semana de ${fmt(primeiro)} a ${fmtFull(ultimo)}${weekOffset===0?' (atual)':''}`;

document.getElementById('mapa-nav').innerHTML = `
  <a class="mini-btn" href="mapa.html?week=${weekOffset-1}">← voltar 7 dias</a>
  <a class="mini-btn" href="mapa.html?week=${weekOffset+1}">avançar 7 dias →</a>
  <a class="mini-btn" href="mapa.html" ${weekOffset===0?'disabled':''}>hoje</a>
`;

document.getElementById('mapa-thead').innerHTML = `<tr>
  <th>Chalé</th><th>Tipo</th>${dias.map(dt=>`<th>${diaSemanaAbrev[dt.getDay()]} ${fmt(dt)}</th>`).join('')}
</tr>`;

document.getElementById('mapa-tbody').innerHTML = rows.map(row=>`
  <tr>
    <td>${row.id}</td>
    <td>${row.tipo}</td>
    ${row.dias.map(s=>`<td><span class="status-pill s-${s}">${STATUS_LABEL[s]}</span></td>`).join('')}
  </tr>`).join('');
