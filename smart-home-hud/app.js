const baseURL = "http://[IP_DEL_ESP32]";
let token = null;

const loginBtn = document.getElementById('login-btn');
const logoutBtn = document.getElementById('logout');
const loginScreen = document.getElementById('login-screen');
const dashboard = document.getElementById('dashboard');
const loginError = document.getElementById('login-error');

async function login() {
  const user = document.getElementById('user').value;
  const pass = document.getElementById('pass').value;
  try {
    const res = await fetch(`${baseURL}/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ user, pass })
    });
    if (!res.ok) throw new Error('credenciales');
    const data = await res.json();
    token = data.token;
    loginScreen.classList.add('hidden');
    dashboard.classList.remove('hidden');
    fetchStatus();
    setInterval(fetchStatus, 3000);
  } catch (e) {
    loginError.classList.remove('hidden');
  }
}

async function fetchStatus() {
  if (!token) return;
  try {
    const res = await fetch(`${baseURL}/status`, {
      headers: { 'Authorization': `Bearer ${token}` }
    });
    const data = await res.json();
    updateRoom('living', data.living);
    updateRoom('kitchen', data.kitchen);
    updateRoom('bedroom', data.bedroom);
  } catch (e) {
    console.error('Estado no disponible', e);
  }
}

function updateRoom(id, info) {
  const room = document.getElementById(id);
  if (info.temp !== undefined) room.querySelector('.temp').textContent = info.temp;
  if (info.hum !== undefined) room.querySelector('.hum').textContent = info.hum;
  if (info.gas !== undefined) room.querySelector('.gas').textContent = info.gas;
  if (info.lum !== undefined) room.querySelector('.lum').textContent = info.lum;
}

async function toggle(endpoint) {
  if (!token) return;
  await fetch(`${baseURL}/toggle-${endpoint}`, {
    headers: { 'Authorization': `Bearer ${token}` }
  });
  fetchStatus();
}

loginBtn.addEventListener('click', login);
logoutBtn.addEventListener('click', () => location.reload());

document.querySelectorAll('.toggle').forEach(btn => {
  btn.addEventListener('click', () => toggle(btn.dataset.endpoint));
});
