const TOFU_AUTH = {
  clientId: '295052813485-8f1rbhmc0gnek9uqh2l4dj9orsk2deni.apps.googleusercontent.com',
  api: 'https://tofuctf-auth.tofu-lab.workers.dev',
};

const authBox = document.getElementById('authBox');
let authCredential = sessionStorage.getItem('tofuctf:googleCredential') || '';
let authUser = null;

function showGoogleButton() {
  authBox.replaceChildren();
  const target = document.createElement('div');
  authBox.append(target);
  google.accounts.id.renderButton(target, {
    type: 'standard', theme: 'outline', size: 'medium', shape: 'rectangular',
    text: 'signin_with', logo_alignment: 'left',
  });
}

function showUser(user) {
  authBox.replaceChildren();
  const chip = document.createElement('div');
  chip.className = 'user-chip';
  if (user.picture && /^https:\/\//.test(user.picture)) {
    const avatar = document.createElement('img');
    avatar.src = user.picture;
    avatar.alt = '';
    avatar.referrerPolicy = 'no-referrer';
    chip.append(avatar);
  }
  const name = document.createElement('span');
  name.textContent = user.name;
  chip.append(name);
  const logout = document.createElement('button');
  logout.type = 'button';
  logout.textContent = 'LOGOUT';
  logout.onclick = signOut;
  chip.append(logout);
  authBox.append(chip);
}

async function verifyCredential(credential) {
  const response = await fetch(`${TOFU_AUTH.api}/auth/google`, {
    method: 'POST',
    headers: { 'content-type': 'application/json' },
    body: JSON.stringify({ credential }),
  });
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || 'Login failed');
  return data.user;
}

async function handleGoogleCredential(response) {
  authBox.innerHTML = '<span class="auth-loading">VERIFYING…</span>';
  try {
    authUser = await verifyCredential(response.credential);
    authCredential = response.credential;
    sessionStorage.setItem('tofuctf:googleCredential', authCredential);
    showUser(authUser);
    window.dispatchEvent(new CustomEvent('tofuctf:login', { detail: authUser }));
  } catch (error) {
    console.error(error);
    authCredential = '';
    sessionStorage.removeItem('tofuctf:googleCredential');
    showGoogleButton();
  }
}

function signOut() {
  google.accounts.id.disableAutoSelect();
  authCredential = '';
  authUser = null;
  sessionStorage.removeItem('tofuctf:googleCredential');
  showGoogleButton();
  window.dispatchEvent(new Event('tofuctf:logout'));
}

async function initGoogleAuth() {
  google.accounts.id.initialize({
    client_id: TOFU_AUTH.clientId,
    callback: handleGoogleCredential,
    auto_select: false,
    cancel_on_tap_outside: true,
  });
  if (authCredential) {
    try {
      authUser = await verifyCredential(authCredential);
      showUser(authUser);
      window.dispatchEvent(new CustomEvent('tofuctf:login', { detail: authUser }));
      return;
    } catch {
      authCredential = '';
      sessionStorage.removeItem('tofuctf:googleCredential');
    }
  }
  showGoogleButton();
}

window.initGoogleAuth = initGoogleAuth;
window.tofuAuth = {
  get user() { return authUser; },
  get credential() { return authCredential; },
};
