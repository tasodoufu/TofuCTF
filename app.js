const dialog = document.querySelector('#flagDialog');
const card = document.querySelector('#warm-tofu');
const state = document.querySelector('.solve-state');
const solved = localStorage.getItem('tofuctf:warm-tofu') === 'solved';

function renderSolved(isSolved) {
  card.classList.toggle('solved', isSolved);
  state.textContent = isSolved ? '✓ Solved locally' : 'Not solved yet';
  document.querySelector('#solvedCount').textContent = isSolved ? '1' : '0';
  document.querySelector('.open-modal').textContent = isSolved ? 'Solved ✓' : 'Submit flag';
}

renderSolved(solved);
document.querySelector('.open-modal').addEventListener('click', () => dialog.showModal());
document.querySelector('#submitFlag').addEventListener('click', () => {
  const value = document.querySelector('#flagInput').value.trim();
  const message = document.querySelector('#flagMessage');
  if (/^TofuCTF\{[a-f0-9]{32}\}$/.test(value)) {
    localStorage.setItem('tofuctf:warm-tofu', 'solved');
    renderSolved(true);
    message.style.color = '#1f5b45';
    message.textContent = 'Correct format — tofu secured!';
    setTimeout(() => dialog.close(), 900);
  } else {
    message.style.color = '#a23b31';
    message.textContent = '形式が違うようです。exploitの出力を確認してみよう。';
  }
});

