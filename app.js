const key=id=>`tofuctf:${id}`,isSolved=id=>localStorage.getItem(key(id))==='solved';
const today=new Intl.DateTimeFormat('sv-SE',{timeZone:'Asia/Tokyo'}).format(new Date());
let items=[],active=null,view=new Date(`${today}T12:00:00+09:00`);
const localSolvedIds=()=>Object.keys(localStorage).filter(k=>k.startsWith('tofuctf:')&&localStorage.getItem(k)==='solved').map(k=>k.slice(8));
async function syncProgress(challengeIds=localSolvedIds()){
  const credential=window.tofuAuth?.credential;
  if(!credential)return false;
  try{
    const response=await fetch(`${TOFU_AUTH.api}/api/progress/import`,{method:'POST',headers:{'content-type':'application/json','authorization':`Bearer ${credential}`},body:JSON.stringify({challengeIds})});
    const data=await response.json();
    if(!response.ok)throw new Error(data.error||'Progress sync failed');
    data.solved.forEach(solve=>localStorage.setItem(key(solve.id),'solved'));
    render();
    window.dispatchEvent(new CustomEvent('tofuctf:progress-synced',{detail:{count:data.solved.length}}));
    return true;
  }catch(error){console.error('TofuCTF progress sync:',error);return false}
}
async function copyLaunchToken(challenge,button){
  const credential=window.tofuAuth?.credential;
  if(!credential){button.textContent='Login required';setTimeout(()=>button.textContent='Copy launch token',1600);return}
  const original=button.textContent;button.disabled=true;button.textContent='ISSUING…';
  try{
    const response=await fetch(`${TOFU_AUTH.api}/api/launch-token`,{method:'POST',headers:{'content-type':'application/json','authorization':`Bearer ${credential}`},body:JSON.stringify({challengeId:challenge.id})});
    const data=await response.json();if(!response.ok)throw new Error(data.error||'Token issue failed');
    await navigator.clipboard.writeText(data.token);button.textContent='Copied — valid 15 min ✓';
  }catch(error){console.error(error);button.textContent='Copy failed — login again'}
  finally{setTimeout(()=>{button.disabled=false;button.textContent=original},2200)}
}
const card=(c,compact=false)=>`<article class="challenge ${isSolved(c.id)?'solved':''} ${compact?'compact':''}" id="${compact?'archive-':''}${c.id}"><div class="challenge-main"><div class="tags"><span>PWN</span><span>${compact?c.topic.toUpperCase():'BEGINNER +1'}</span><span>${c.points} PTS</span></div><h3>${c.title}</h3><p>${c.subtitle}</p><dl><div><dt>ARCH</dt><dd>${c.arch}</dd></div><div><dt>MITIGATIONS</dt><dd>${c.mitigations}</dd></div><div><dt>ACCESS</dt><dd>localhost:${c.port}</dd></div></dl></div><aside class="challenge-side"><div class="difficulty"><span>DIFFICULTY</span><b>${'●'.repeat(c.level)}${'○'.repeat(5-c.level)}</b></div><a class="download" href="${c.download}" download>${compact?'Download':'Download challenge'} <span>↓</span></a><button class="open-modal" data-id="${c.id}" type="button">${isSolved(c.id)?'Solved ✓':'Submit flag'}</button><small class="solve-state">${isSolved(c.id)?'✓ Solved locally':'Not solved yet'}</small></aside></article>`;
function bind(){document.querySelectorAll('.open-modal').forEach(b=>b.onclick=()=>openChallenge(items.find(c=>c.id===b.dataset.id)))}
function render(){const available=items.filter(c=>c.date<=today),latest=available.find(c=>c.date===today)||available.at(-1);todayChallenge.innerHTML=latest?card(latest):'<p>最初の問題を準備中。</p>';solvedCount.textContent=available.filter(c=>isSolved(c.id)).length;availableCount.textContent=String(available.length).padStart(2,'0');comebackCount.textContent=localStorage.getItem('tofuctf:comebacks')||0;bind();renderCalendar()}
function openChallenge(c){active=c;challengeDialogKicker.textContent=`CHALLENGE ${String(c.number).padStart(3,'0')} / ${c.date}`;challengeDialogTitle.textContent=c.title;challengeDialogDescription.textContent=c.subtitle;challengeDialogMeta.innerHTML=`<div><span>TOPIC</span><b>${c.topic}</b></div><div><span>DIFFICULTY</span><b>${'●'.repeat(c.level)}${'○'.repeat(5-c.level)}</b></div><div><span>ARCH</span><b>${c.arch}</b></div><div><span>ACCESS</span><b>localhost:${c.port}</b></div>`;challengeDialogDownload.href=c.download;challengeDialogSubmit.textContent=isSolved(c.id)?'Solved ✓':'Submit flag';challengeDialog.showModal()}
function renderCalendar(){const y=view.getFullYear(),m=view.getMonth(),offset=(new Date(y,m,1).getDay()+6)%7,days=new Date(y,m+1,0).getDate(),cells=Array(offset).fill('<span class="calendar-day blank"></span>');monthLabel.textContent=`${y}.${String(m+1).padStart(2,'0')}`;for(let d=1;d<=days;d++){const date=`${y}-${String(m+1).padStart(2,'0')}-${String(d).padStart(2,'0')}`,c=items.find(x=>x.date===date),kind=c?(isSolved(c.id)?'solved':date<=today?'available':'rest'):'rest';cells.push(`<button class="calendar-day ${kind}" ${c&&date<=today?`data-challenge="${c.id}"`:'disabled'}><b>${d}</b>${c?`<span class="calendar-title">${c.title}</span><small>#${String(c.number).padStart(2,'0')}</small>`:''}</button>`)}calendarGrid.innerHTML=cells.join('');document.querySelectorAll('[data-challenge]').forEach(b=>b.onclick=()=>openChallenge(items.find(c=>c.id===b.dataset.challenge)))}
submitFlag.onclick=async()=>{
  if(!active||!/^TofuCTF\{[a-f0-9]{32}\}$/.test(flagInput.value.trim())){flagMessage.textContent='形式が違うようです。exploitの出力を確認しよう。';flagMessage.style.color='#a23b31';return}
  const credential=window.tofuAuth?.credential;if(!credential){flagMessage.textContent='本物の判定にはGoogleログインが必要です。';flagMessage.style.color='#a23b31';return}
  submitFlag.disabled=true;flagMessage.textContent='VERIFYING…';flagMessage.style.color='#77736a';
  try{
    const response=await fetch(`${TOFU_AUTH.api}/api/submit`,{method:'POST',headers:{'content-type':'application/json','authorization':`Bearer ${credential}`},body:JSON.stringify({challengeId:active.id,flag:flagInput.value.trim()})});
    const data=await response.json();if(!response.ok)throw new Error(data.error||'Submit failed');
    if(!data.correct){flagMessage.textContent='Incorrect — flagが一致しません。';flagMessage.style.color='#a23b31';return}
    localStorage.setItem(key(active.id),'solved');data.solved.forEach(solve=>localStorage.setItem(key(solve.id),'solved'));render();flagMessage.textContent='Correct — アカウントに保存しました！';flagMessage.style.color='#1f5b45';setTimeout(()=>flagDialog.close(),900);
  }catch(error){console.error(error);flagMessage.textContent='判定できませんでした。ログイン状態と通信を確認してください。';flagMessage.style.color='#a23b31'}finally{submitFlag.disabled=false}
};
focusButton.onclick=()=>focusDialog.showModal();prevMonth.onclick=()=>{view.setMonth(view.getMonth()-1);renderCalendar()};nextMonth.onclick=()=>{view.setMonth(view.getMonth()+1);renderCalendar()};currentMonth.onclick=()=>{view=new Date(`${today}T12:00:00+09:00`);renderCalendar()};challengeDialogLaunch.onclick=()=>copyLaunchToken(active,challengeDialogLaunch);challengeDialogSubmit.onclick=()=>{challengeDialog.close();dialogKicker.textContent=`CHALLENGE ${String(active.number).padStart(3,'0')}`;dialogTitle.textContent=active.title;flagInput.value='';flagMessage.textContent='';flagDialog.showModal()};pickBacklog.onclick=()=>{const backlog=items.filter(c=>c.date<=today&&!isSolved(c.id)),pick=backlog[Math.floor(Math.random()*backlog.length)];if(pick){localStorage.setItem('tofuctf:comebacks',String(Number(localStorage.getItem('tofuctf:comebacks')||0)+1));openChallenge(pick);render()}};
fetch('challenges.json').then(r=>r.json()).then(data=>{items=data.sort((a,b)=>a.date.localeCompare(b.date));render()}).catch(()=>todayChallenge.textContent='問題一覧を読み込めませんでした。');
window.addEventListener('tofuctf:login',()=>syncProgress());
