// ==========================================================================
// ESP8266 RTOS Prism Controller - Mobile Application Logic
// Wireless Control, Dynamic Top Island Animation, Live Wi-Fi dBm & Channel Diagnostics
// ==========================================================================

let espHost = localStorage.getItem('esp_host') || (window.location.hostname && window.location.hostname !== 'localhost' ? window.location.hostname : '192.168.4.1');
let relayData = {};
let scheduleDays = { 1: 0xFF, 2: 0xFF, 3: 0xFF, 4: 0xFF };
let schedulesLoaded = false;
let isConnected = false;
let activeTabs = { 1: 'timer', 2: 'timer', 3: 'timer', 4: 'timer' };

const pinLabels = {
  1: 'D1 / GPIO 5',
  2: 'D2 / GPIO 4',
  3: 'D5 / GPIO 14',
  4: 'D6 / GPIO 12'
};

const dayNames = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

// Utility: Trigger Short Haptic Feedback on Mobile
function triggerHaptic(duration = 14) {
  if (navigator.vibrate) {
    try { navigator.vibrate(duration); } catch (e) {}
  }
}

// Utility: Show Floating Toast
function showToast(msg) {
  const toast = document.getElementById('appToast');
  if (!toast) return;
  toast.innerText = msg;
  toast.classList.add('show');
  setTimeout(() => toast.classList.remove('show'), 2600);
}

// Format API Base URL
function getApiUrl(endpoint) {
  let host = espHost.trim();
  if (!host.startsWith('http://') && !host.startsWith('https://')) {
    host = 'http://' + host;
  }
  if (host.endsWith('/')) {
    host = host.slice(0, -1);
  }
  return `${host}${endpoint}`;
}

// Requirement 1: Connection Guard - Intercepts actions if disconnected
function assertConnected() {
  if (!isConnected) {
    triggerHaptic(30);
    showToast('⚠️ ESP8266 Disconnected! Connect to Wi-Fi first.');
    openModal('connRequiredModal');
    return false;
  }
  return true;
}

// Requirement 1: Handshake Test & Auto-Forward
async function runHandshakeTest() {
  triggerHaptic(12);
  const spinner = document.getElementById('handshakeSpinner');
  const btn = document.getElementById('btnTestHandshake');
  if (spinner) spinner.style.display = 'inline-block';
  if (btn) btn.disabled = true;

  try {
    const controller = new AbortController();
    const timeoutId = setTimeout(() => controller.abort(), 3000);

    const res = await fetch(getApiUrl('/api/handshake'), {
      cache: 'no-store',
      signal: controller.signal
    });
    clearTimeout(timeoutId);

    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();

    if (data.verified && data.system === 'ESP8266_RTOS') {
      isConnected = true;
      triggerHaptic(25);
      showToast('✅ Handshake Verified! Welcome to ESP Prism.');
      closeModal('connRequiredModal');
      closeModal('connModal');
      pollStatus();
    } else {
      throw new Error('Invalid signature');
    }
  } catch (err) {
    triggerHaptic(40);
    showToast('❌ Handshake Failed! Ensure phone is on "Yash_RTOS_System\'s"');
  } finally {
    if (spinner) spinner.style.display = 'none';
    if (btn) btn.disabled = false;
  }
}

// Requirement 3: Build 4 Channel Cards with Controls NOT MINIMISED (Always Open)
function buildChannelCards() {
  const container = document.getElementById('channelsContainer');
  let html = '';

  for (let ch = 1; ch <= 4; ch++) {
    html += `
    <div class="channel-card" id="cardCh${ch}">
      <!-- Channel Header -->
      <div class="channel-header">
        <div class="channel-left">
          <div class="channel-badge" id="badgeCh${ch}">R${ch}</div>
          <div class="channel-meta">
            <h4>
              <span id="titleCh${ch}">Relay ${ch}</span>
              <span class="badge-pill pill-countdown" id="pillCountdown${ch}" style="display:none;">00:00</span>
              <span class="badge-pill pill-schedule" id="pillSched${ch}" style="display:none;">📅 Sched</span>
            </h4>
            <p>
              <span>${pinLabels[ch]}</span> &bull;
              <span id="wattsCh${ch}">100W</span> &bull;
              <span id="stateLabelCh${ch}">OFF</span>
            </p>
          </div>
        </div>
        <div class="apple-switch" id="switchCh${ch}" onclick="toggleRelay(${ch})"></div>
      </div>

      <!-- Controls Pane (ALWAYS OPEN / NOT MINIMISED) -->
      <div class="channel-controls-pane">
        <!-- Segmented Tab Selector -->
        <div class="channel-tabs">
          <button class="tab-btn ${activeTabs[ch] === 'timer' ? 'active' : ''}" id="tabBtnTimer_${ch}" onclick="switchTab(${ch}, 'timer')">⏱ Timer</button>
          <button class="tab-btn ${activeTabs[ch] === 'cycle' ? 'active' : ''}" id="tabBtnCycle_${ch}" onclick="switchTab(${ch}, 'cycle')">🔁 Loop</button>
          <button class="tab-btn ${activeTabs[ch] === 'sched' ? 'active' : ''}" id="tabBtnSched_${ch}" onclick="switchTab(${ch}, 'sched')">📅 Schedule</button>
          <button class="tab-btn ${activeTabs[ch] === 'settings' ? 'active' : ''}" id="tabBtnSettings_${ch}" onclick="switchTab(${ch}, 'settings')">⚡ Config</button>
        </div>

        <!-- 1. Timer Pane -->
        <div class="tab-pane ${activeTabs[ch] === 'timer' ? 'active' : ''}" id="tabPaneTimer_${ch}">
          <div class="chips-row">
            <button class="chip-btn" onclick="setTimerPreset(${ch}, 60)">+1 min</button>
            <button class="chip-btn" onclick="setTimerPreset(${ch}, 300)">+5 min</button>
            <button class="chip-btn" onclick="setTimerPreset(${ch}, 900)">+15 min</button>
            <button class="chip-btn" onclick="setTimerPreset(${ch}, 1800)">+30 min</button>
            <button class="chip-btn" onclick="setTimerPreset(${ch}, 3600)">+1 hr</button>
          </div>
          <div class="form-row">
            <div class="input-col">
              <label>Delay (Sec)</label>
              <input type="number" class="app-input" id="timerDelay_${ch}" value="0" min="0">
            </div>
            <div class="input-col">
              <label>ON Duration (Sec)</label>
              <input type="number" class="app-input" id="timerDuration_${ch}" value="60" min="1">
            </div>
          </div>
          <div class="btn-row">
            <button class="btn-primary" onclick="startTimer(${ch})">Start Timer</button>
            <button class="btn-danger" onclick="cancelTimer(${ch})">Stop</button>
          </div>
        </div>

        <!-- 2. Cycle Pane -->
        <div class="tab-pane ${activeTabs[ch] === 'cycle' ? 'active' : ''}" id="tabPaneCycle_${ch}">
          <div class="form-row">
            <div class="input-col">
              <label>ON Time (Sec)</label>
              <input type="number" class="app-input" id="cycleOn_${ch}" value="30" min="1">
            </div>
            <div class="input-col">
              <label>OFF Time (Sec)</label>
              <input type="number" class="app-input" id="cycleOff_${ch}" value="30" min="1">
            </div>
          </div>
          <div class="form-row">
            <div class="input-col">
              <label>Repeat Count (0 = Infinite)</label>
              <input type="number" class="app-input" id="cycleCount_${ch}" value="0" min="0">
            </div>
          </div>
          <div class="btn-row">
            <button class="btn-primary" onclick="startCycle(${ch})">Start Loop</button>
            <button class="btn-danger" onclick="cancelCycle(${ch})">Stop</button>
          </div>
        </div>

        <!-- 3. Schedule Pane -->
        <div class="tab-pane ${activeTabs[ch] === 'sched' ? 'active' : ''}" id="tabPaneSched_${ch}">
          <div class="form-row">
            <div class="input-col">
              <label>Start Time (24h)</label>
              <input type="time" class="app-input" id="schedStart_${ch}" value="08:00">
            </div>
            <div class="input-col">
              <label>End Time (24h)</label>
              <input type="time" class="app-input" id="schedEnd_${ch}" value="18:00">
            </div>
          </div>
          <div class="input-col">
            <label>Active Days</label>
            <div class="days-grid" id="daysGrid_${ch}">
              ${dayNames.map((d, idx) => `<div class="day-pill active" id="dayPill_${ch}_${idx}" onclick="toggleDayPill(${ch}, ${idx})">${d}</div>`).join('')}
            </div>
          </div>
          <div class="btn-row">
            <button class="btn-primary" onclick="saveSchedule(${ch})">Save Schedule</button>
            <button class="btn-danger" onclick="cancelSchedule(${ch})">Disable</button>
          </div>
        </div>

        <!-- 4. Settings Pane -->
        <div class="tab-pane ${activeTabs[ch] === 'settings' ? 'active' : ''}" id="tabPaneSettings_${ch}">
          <div class="form-row">
            <div class="input-col">
              <label>Channel Name</label>
              <input type="text" class="app-input" id="cfgName_${ch}" value="Relay ${ch}">
            </div>
            <div class="input-col">
              <label>Load Power (Watts)</label>
              <input type="number" class="app-input" id="cfgWatts_${ch}" value="100" min="1">
            </div>
          </div>
          <div class="form-row">
            <div class="input-col">
              <label>Trigger Logic</label>
              <select class="app-input" id="cfgActLow_${ch}">
                <option value="1">Active LOW (Standard)</option>
                <option value="0">Active HIGH</option>
              </select>
            </div>
            <div class="input-col">
              <label>Power-On Default</label>
              <select class="app-input" id="cfgPwrOn_${ch}">
                <option value="0">Always Boot OFF</option>
                <option value="1">Always Boot ON</option>
                <option value="2">Restore Last State</option>
              </select>
            </div>
          </div>
          <button class="btn-primary" style="width:100%;" onclick="saveChannelSettings(${ch})">Save Channel Settings</button>
        </div>

      </div>
    </div>`;
  }

  container.innerHTML = html;
}

// Switch Tab in Card
function switchTab(ch, tabName) {
  triggerHaptic(8);
  activeTabs[ch] = tabName;
  const card = document.getElementById(`cardCh${ch}`);
  if (!card) return;

  card.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  card.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));

  const btn = document.getElementById(`tabBtn${tabName.charAt(0).toUpperCase() + tabName.slice(1)}_${ch}`);
  const pane = document.getElementById(`tabPane${tabName.charAt(0).toUpperCase() + tabName.slice(1)}_${ch}`);
  if (btn) btn.classList.add('active');
  if (pane) pane.classList.add('active');
}

// Toggle Day Pill
function toggleDayPill(ch, dayIdx) {
  const pill = document.getElementById(`dayPill_${ch}_${dayIdx}`);
  const bit = 1 << dayIdx;
  scheduleDays[ch] ^= bit;
  if (pill) pill.classList.toggle('active', (scheduleDays[ch] & bit) !== 0);
  triggerHaptic(8);
}

// Timer preset shortcut
function setTimerPreset(ch, secs) {
  document.getElementById(`timerDuration_${ch}`).value = secs;
  showToast(`Duration: ${secs >= 60 ? secs/60 + ' min' : secs + ' sec'}`);
  triggerHaptic(8);
}

// Visual update for channel
function updateChannelVisual(ch, state) {
  const card = document.getElementById(`cardCh${ch}`);
  const lbl = document.getElementById(`stateLabelCh${ch}`);
  if (card) card.classList.toggle('active', state);
  if (lbl) lbl.innerText = state ? 'ACTIVE (ON)' : 'STANDBY (OFF)';
}

// Format Seconds to MM:SS
function formatSec(sec) {
  const m = Math.floor(sec / 60);
  const s = sec % 60;
  return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
}

// Requirement 4 & 5: Live Status Polling & Top Dynamic Island Animation
async function pollStatus() {
  const island = document.getElementById('topDynamicIsland');
  const islandDot = document.getElementById('islandDot');
  const islandRing = document.getElementById('islandRing');
  const islandStatus = document.getElementById('islandStatusText');
  const islandMetrics = document.getElementById('islandMetricsText');
  const signalBars = document.getElementById('islandSignalBars');
  const offlineBanner = document.getElementById('offlineAlertBanner');

  try {
    const controller = new AbortController();
    const timeoutId = setTimeout(() => controller.abort(), 2500);

    const res = await fetch(getApiUrl('/api/status'), {
      cache: 'no-store',
      signal: controller.signal
    });
    clearTimeout(timeoutId);

    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();

    // Handshake Success / Connected State
    isConnected = true;
    if (offlineBanner) offlineBanner.style.display = 'none';

    // Requirement 4: Top Dynamic Island Animation (Connected)
    if (island) island.className = 'dynamic-island connected';
    if (islandDot) islandDot.className = 'radar-dot online';
    if (islandRing) islandRing.className = 'radar-ring online';
    if (islandStatus) islandStatus.innerText = 'ESP8266 RTOS';

    // Requirement 5: Signal Strength & RF Channel Diagnostics
    let rssi = -45;
    let ch = 1;
    let freq = 2412;
    let clients = 1;

    if (data.wifi) {
      rssi = data.wifi.rssi || -45;
      ch = data.wifi.channel || 1;
      freq = data.wifi.freq_mhz || (2407 + ch * 5);
      clients = data.wifi.clients || 1;
    }

    // Determine signal bars and quality badge
    let qualityBadge = 'Strong';
    let sigClass = 'sig-4';

    if (rssi >= -55) {
      qualityBadge = 'Strong';
      sigClass = 'sig-4';
    } else if (rssi >= -68) {
      qualityBadge = 'Good';
      sigClass = 'sig-3';
    } else if (rssi >= -80) {
      qualityBadge = 'Fair';
      sigClass = 'sig-2';
    } else {
      qualityBadge = 'Weak';
      sigClass = 'sig-1';
    }

    if (signalBars) signalBars.className = `signal-bars ${sigClass}`;
    if (islandMetrics) islandMetrics.innerText = `${rssi} dBm • CH ${ch}`;

    // Update Hero Card Wi-Fi Diagnostics
    const heroSig = document.getElementById('heroSignalDbm');
    const heroQual = document.getElementById('heroSignalQuality');
    const heroCh = document.getElementById('heroWifiChannel');
    const heroFreq = document.getElementById('heroWifiFreq');
    const heroClients = document.getElementById('heroWifiClients');

    if (heroSig) heroSig.innerText = `${rssi} dBm`;
    if (heroQual) {
      heroQual.innerText = qualityBadge;
      heroQual.style.background = rssi >= -68 ? 'rgba(16, 185, 129, 0.25)' : 'rgba(245, 158, 11, 0.25)';
      heroQual.style.borderColor = rssi >= -68 ? '#10b981' : '#f59e0b';
    }
    if (heroCh) heroCh.innerText = `CH ${ch}`;
    if (heroFreq) heroFreq.innerText = `${freq} MHz`;
    if (heroClients) heroClients.innerText = `${clients} Client${clients > 1 ? 's' : ''}`;

    // Update Diagnostics Modal
    const dStatus = document.getElementById('diagConnStatus');
    const dSig = document.getElementById('diagSignalDbm');
    const dCh = document.getElementById('diagChannel');
    const dFreq = document.getElementById('diagFreq');
    const dUp = document.getElementById('diagUptime');
    const dHeap = document.getElementById('diagHeap');

    if (dStatus) dStatus.innerText = 'Connected & Synchronized';
    if (dSig) dSig.innerText = `${rssi} dBm (${qualityBadge})`;
    if (dCh) dCh.innerText = `Channel ${ch}`;
    if (dFreq) dFreq.innerText = `${freq} MHz`;

    // 1. Clock
    if (data.time) {
      document.getElementById('navClock').innerText = data.time.clock || '--:--:--';
      document.getElementById('navDate').innerText = data.time.date || 'Synchronized';
    }

    // 2. Channels
    let totalWatts = 0;
    let lifetimeKwh = 0;

    for (let c = 1; c <= 4; c++) {
      const r = data[`r${c}`];
      if (r) {
        relayData[`r${c}`] = r;
        updateChannelVisual(c, r.state);

        const title = document.getElementById(`titleCh${c}`);
        const watts = document.getElementById(`wattsCh${c}`);
        if (title) title.innerText = r.name;
        if (watts) watts.innerText = r.watts.toFixed(0) + 'W';

        if (r.state) totalWatts += r.watts;
        lifetimeKwh += (r.total_sec / 3600.0) * (r.watts / 1000.0);

        // Countdown pill
        const pill = document.getElementById(`pillCountdown${c}`);
        if (pill) {
          if (r.timer && r.timer.active) {
            pill.style.display = 'inline-block';
            pill.innerText = '⏱ ' + (r.timer.in_delay ? 'Delay: ' : 'ON: ') + formatSec(r.timer.remaining);
          } else if (r.cycle && r.cycle.active) {
            pill.style.display = 'inline-block';
            pill.innerText = '🔁 ' + (r.cycle.in_on ? 'ON: ' : 'OFF: ') + formatSec(r.cycle.remaining);
          } else {
            pill.style.display = 'none';
          }
        }

        // Schedule pill
        const sPill = document.getElementById(`pillSched${c}`);
        if (sPill) {
          if (r.schedule && r.schedule.enabled) {
            sPill.style.display = 'inline-block';
            const sh = String(r.schedule.start_h).padStart(2, '0');
            const sm = String(r.schedule.start_m).padStart(2, '0');
            const eh = String(r.schedule.end_h).padStart(2, '0');
            const em = String(r.schedule.end_m).padStart(2, '0');
            sPill.innerText = `📅 ${sh}:${sm}-${eh}:${em}`;
          } else {
            sPill.style.display = 'none';
          }
        }

        // Sync inputs on first load
        if (!schedulesLoaded && r.schedule) {
          const sInp = document.getElementById(`schedStart_${c}`);
          const eInp = document.getElementById(`schedEnd_${c}`);
          if (sInp) sInp.value = `${String(r.schedule.start_h).padStart(2, '0')}:${String(r.schedule.start_m).padStart(2, '0')}`;
          if (eInp) eInp.value = `${String(r.schedule.end_h).padStart(2, '0')}:${String(r.schedule.end_m).padStart(2, '0')}`;
          scheduleDays[c] = r.schedule.days;
          for (let d = 0; d < 7; d++) {
            const p = document.getElementById(`dayPill_${c}_${d}`);
            if (p) p.classList.toggle('active', (scheduleDays[c] & (1 << d)) !== 0);
          }
        }
      }
    }

    schedulesLoaded = true;
    document.getElementById('totalWatts').innerText = `${totalWatts.toFixed(0)} W`;
    document.getElementById('totalKwh').innerText = `${lifetimeKwh.toFixed(3)} kWh Total`;

    // 3. Telemetry
    if (data.telem) {
      const upH = Math.floor(data.telem.uptime_sec / 3600);
      const upM = Math.floor((data.telem.uptime_sec % 3600) / 60);
      if (dUp) dUp.innerText = `${upH}h ${upM}m`;
      if (dHeap) dHeap.innerText = (data.telem.free_heap / 1024).toFixed(1) + ' KB';
    }

    // RTOS Kernel Status & Exception Handler
    const alertBox = document.getElementById('mobileKernelAlert');
    if (data.kernel) {
      if (data.kernel.fault) {
        if (alertBox) {
          alertBox.style.display = 'flex';
          const title = document.getElementById('mobileKernelTitle');
          const msg = document.getElementById('mobileKernelMsg');
          if (title) title.innerText = `🚨 Kernel [${data.kernel.severity || 'FAULT'}] ${data.kernel.code || ''}`;
          if (msg) msg.innerText = `${data.kernel.msg || 'Exception occurred'} (Total: ${data.kernel.count || 1})`;
        }
      } else {
        if (alertBox) alertBox.style.display = 'none';
      }
    }

    // 4. Logs
    if (data.logs && Array.isArray(data.logs)) {
      const box = document.getElementById('logsContainer');
      if (box) {
        box.innerHTML = data.logs.map(l => `<div class="log-line">[${l.time}] ${l.msg}</div>`).join('');
      }
    }

  } catch (err) {
    // Disconnected / Offline State
    isConnected = false;
    if (offlineBanner) offlineBanner.style.display = 'flex';

    if (island) island.className = 'dynamic-island offline';
    if (islandDot) islandDot.className = 'radar-dot offline';
    if (islandRing) islandRing.className = 'radar-ring offline';
    if (islandStatus) islandStatus.innerText = 'Offline';
    if (islandMetrics) islandMetrics.innerText = 'Tap to Connect';
    if (signalBars) signalBars.className = 'signal-bars';

    const dStatus = document.getElementById('diagConnStatus');
    if (dStatus) dStatus.innerText = 'Disconnected (Check Wi-Fi)';
  }
}

// Clear RTOS Kernel Exception
async function clearKernelFault() {
  if (!assertConnected()) return;
  triggerHaptic(15);
  try {
    await fetch(getApiUrl('/api/kernel/clear'), { method: 'POST' });
    showToast('RTOS Kernel Fault Cleared');
    const alertBox = document.getElementById('mobileKernelAlert');
    if (alertBox) alertBox.style.display = 'none';
    pollStatus();
  } catch (err) {
    showToast('Error clearing kernel fault');
  }
}

// Relay Instant Toggle (With Connection Guard)
async function toggleRelay(ch) {
  if (!assertConnected()) return;
  triggerHaptic(18);
  const cur = relayData[`r${ch}`] ? relayData[`r${ch}`].state : false;
  const target = !cur;

  // Optimistic UI update
  updateChannelVisual(ch, target);

  try {
    await fetch(getApiUrl('/api/relay'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&state=${target ? 1 : 0}`
    });
    showToast(`Relay ${ch} switched ${target ? 'ON' : 'OFF'}`);
  } catch (err) {
    showToast('Failed to switch relay');
    pollStatus();
  }
}

// Master All ON / All OFF (With Connection Guard)
async function triggerMasterAll(on) {
  if (!assertConnected()) return;
  triggerHaptic(24);
  for (let ch = 1; ch <= 4; ch++) updateChannelVisual(ch, on);

  try {
    await fetch(getApiUrl(on ? '/api/all_on' : '/api/all_off'), { method: 'POST' });
    showToast(on ? '⚡ All 4 Relays Switched ON' : '🚨 All Relays Switched OFF');
  } catch (err) {
    showToast('Failed to toggle all relays');
  }
}

// Schedules
async function saveSchedule(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  const start = document.getElementById(`schedStart_${ch}`).value || '08:00';
  const end = document.getElementById(`schedEnd_${ch}`).value || '18:00';
  const [sh, sm] = start.split(':').map(Number);
  const [eh, em] = end.split(':').map(Number);
  const days = scheduleDays[ch];

  try {
    await fetch(getApiUrl('/api/schedule'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&start_h=${sh}&start_m=${sm}&end_h=${eh}&end_m=${em}&days=${days}&enable=1`
    });
    showToast(`Schedule saved for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error saving schedule');
  }
}

async function cancelSchedule(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  try {
    await fetch(getApiUrl('/api/schedule'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&enable=0`
    });
    showToast(`Schedule disabled for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error disabling schedule');
  }
}

// Timers
async function startTimer(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  const delay = parseInt(document.getElementById(`timerDelay_${ch}`).value) || 0;
  const duration = parseInt(document.getElementById(`timerDuration_${ch}`).value) || 60;

  try {
    await fetch(getApiUrl('/api/timer'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&start_delay=${delay}&duration=${duration}&enable=1`
    });
    showToast(`Timer started for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error starting timer');
  }
}

async function cancelTimer(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  try {
    await fetch(getApiUrl('/api/timer'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&enable=0`
    });
    showToast(`Timer cancelled for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error cancelling timer');
  }
}

// Cycle Loop
async function startCycle(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  const onSec = parseInt(document.getElementById(`cycleOn_${ch}`).value) || 30;
  const offSec = parseInt(document.getElementById(`cycleOff_${ch}`).value) || 30;
  const count = parseInt(document.getElementById(`cycleCount_${ch}`).value) || 0;

  try {
    await fetch(getApiUrl('/api/cycle'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&on_sec=${onSec}&off_sec=${offSec}&cycles=${count}&enable=1`
    });
    showToast(`Cycle loop started for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error starting cycle');
  }
}

async function cancelCycle(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  try {
    await fetch(getApiUrl('/api/cycle'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `id=${ch}&enable=0`
    });
    showToast(`Cycle loop stopped for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error stopping cycle');
  }
}

// Channel Settings
async function saveChannelSettings(ch) {
  if (!assertConnected()) return;
  triggerHaptic(12);
  const name = encodeURIComponent(document.getElementById(`cfgName_${ch}`).value.trim());
  const watts = parseFloat(document.getElementById(`cfgWatts_${ch}`).value) || 100.0;
  const actLow = document.getElementById(`cfgActLow_${ch}`).value;
  const pwrOn = document.getElementById(`cfgPwrOn_${ch}`).value;

  try {
    await fetch(getApiUrl('/api/settings'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `r${ch}_name=${name}&r${ch}_watts=${watts}&r${ch}_actlow=${actLow}&r${ch}_pwron=${pwrOn}`
    });
    showToast(`Settings saved for Relay ${ch}`);
    pollStatus();
  } catch (err) {
    showToast('Error saving channel settings');
  }
}

// Phone Time Synchronization
async function syncPhoneTime() {
  if (!assertConnected()) return;
  triggerHaptic(14);
  const nowEpoch = Math.floor(Date.now() / 1000);
  const tzOffsetMinutes = -new Date().getTimezoneOffset();

  try {
    await fetch(getApiUrl('/api/time/sync'), {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `epoch=${nowEpoch}&tz_offset=${tzOffsetMinutes}`
    });
    showToast('Phone Time Synchronized with ESP!');
    pollStatus();
  } catch (err) {
    showToast('Time sync failed! Check Wi-Fi connection.');
  }
}

// Modal Dialog Helpers
function openModal(id) {
  const m = document.getElementById(id);
  if (m) m.classList.add('open');
}

function closeModal(id) {
  const m = document.getElementById(id);
  if (m) m.classList.remove('open');
}

function closeModalOnBg(e, id) {
  if (e.target.id === id) closeModal(id);
}

function setHostPreset(preset) {
  document.getElementById('cfgEspHost').value = preset;
}

function saveHostConfig() {
  const val = document.getElementById('cfgEspHost').value.trim();
  if (val) {
    espHost = val;
    localStorage.setItem('esp_host', espHost);
    schedulesLoaded = false;
    showToast(`Target Host Set: ${espHost}`);
    runHandshakeTest();
  }
}

// Startup Initialization
window.addEventListener('DOMContentLoaded', () => {
  document.getElementById('cfgEspHost').value = espHost;
  buildChannelCards();
  pollStatus();
  setInterval(pollStatus, 1500);

  // Attempt initial handshake test
  setTimeout(runHandshakeTest, 500);

  // Register PWA Service Worker
  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('./sw.js').then(() => {
      console.log('ESP Relay PWA Service Worker Registered');
    }).catch(err => console.log('SW registration error:', err));
  }
});
