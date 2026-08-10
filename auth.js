const TOFU_AUTH = {
  clientId: '295052813485-8f1rbhmc0gnek9uqh2l4dj9orsk2deni.apps.googleusercontent.com',
  api: 'https://tofuctf-auth.tofu-lab.workers.dev',
};

const authBox = document.getElementById('authBox');
let authCredential = sessionStorage.getItem('tofuctf:googleCredential') || '';
let authUser = null;
let profileAbort = new AbortController();

function showGoogleButton() {
  profileAbort.abort();
  profileAbort = new AbortController();
  authBox.replaceChildren();
  const target = document.createElement('div');
  authBox.append(target);
  google.accounts.id.renderButton(target, {
    type: 'standard', theme: 'outline', size: 'medium', shape: 'rectangular',
    text: 'signin_with', logo_alignment: 'left',
  });
}

function showUser(user) {
  profileAbort.abort();
  profileAbort = new AbortController();
  authBox.replaceChildren();
  const profile = document.createElement('div');
  profile.className = 'user-profile';

  const trigger = document.createElement('button');
  trigger.type = 'button';
  trigger.className = 'avatar-button';
  trigger.setAttribute('aria-label', 'アカウントメニューを開く');
  trigger.setAttribute('aria-haspopup', 'menu');
  trigger.setAttribute('aria-expanded', 'false');
  if (user.picture && /^https:\/\//.test(user.picture)) {
    const avatar = document.createElement('img');
    avatar.src = user.picture;
    avatar.alt = `${user.name}のプロフィール画像`;
    avatar.referrerPolicy = 'no-referrer';
    trigger.append(avatar);
  } else {
    const fallback = document.createElement('span');
    fallback.className = 'avatar-fallback';
    fallback.textContent = (user.name || '?').trim().charAt(0).toUpperCase();
    trigger.append(fallback);
  }

  const menu = document.createElement('div');
  menu.className = 'account-menu';
  menu.setAttribute('role', 'menu');
  menu.hidden = true;

  const name = document.createElement('span');
  name.className = 'account-name';
  name.textContent = user.name;
  menu.append(name);

  if (user.email) {
    const email = document.createElement('span');
    email.className = 'account-email';
    email.textContent = user.email;
    menu.append(email);
  }

  const logout = document.createElement('button');
  logout.type = 'button';
  logout.className = 'account-logout';
  logout.setAttribute('role', 'menuitem');
  logout.textContent = 'ログアウト';
  logout.onclick = signOut;
  menu.append(logout);

  const closeMenu = () => {
    menu.hidden = true;
    trigger.setAttribute('aria-expanded', 'false');
  };
  trigger.onclick = () => {
    const willOpen = menu.hidden;
    menu.hidden = !willOpen;
    trigger.setAttribute('aria-expanded', String(willOpen));
  };
  profile.onkeydown = (event) => {
    if (event.key === 'Escape') {
      closeMenu();
      trigger.focus();
    }
  };
  document.addEventListener('click', (event) => {
    if (!profile.contains(event.target)) closeMenu();
  }, { signal: profileAbort.signal });

  profile.append(trigger, menu);
  authBox.append(profile);
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
