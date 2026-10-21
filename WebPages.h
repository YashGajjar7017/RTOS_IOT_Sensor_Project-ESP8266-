#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <Arduino.h>

// =====================================================================
// DEDICATED PORT 500 OTA FIRMWARE UPDATE PORTAL HTML
// =====================================================================
const char OTA_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>AURA OTA Firmware Station (Port 500)</title>
  <style>
    :root {
      --bg: #07090e;
      --card-glass: rgba(18, 24, 38, 0.65);
      --card-border: rgba(255, 255, 255, 0.14);
      --prism-rim: inset 0 1px 1px 0 rgba(255, 255, 255, 0.28);
      --primary: #38bdf8;
      --primary-glow: rgba(56, 189, 248, 0.4);
      --success: #10b981;
      --success-glow: rgba(16, 185, 129, 0.4);
      --danger: #ff3b30;
      --text: #f8fafc;
      --text-sub: #94a3b8;
      --text-muted: #64748b;
      --radius-lg: 24px;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "SF Pro Display", "Segoe UI", Roboto, sans-serif; }
    body {
      background-color: var(--bg);
      background-image:
        radial-gradient(circle at 20% 15%, rgba(99, 102, 241, 0.18), transparent 45%),
        radial-gradient(circle at 80% 25%, rgba(56, 189, 248, 0.15), transparent 40%),
        radial-gradient(circle at 50% 85%, rgba(16, 185, 129, 0.12), transparent 45%);
      background-attachment: fixed;
      color: var(--text);
      min-height: 100vh;
      padding: 24px 16px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
    }
    .container { width: 100%; max-width: 500px; display: flex; flex-direction: column; gap: 18px; }
    .glass-card {
      background: var(--card-glass);
      backdrop-filter: blur(28px) saturate(200%);
      -webkit-backdrop-filter: blur(28px) saturate(200%);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-lg);
      box-shadow: 0 20px 50px -10px rgba(0,0,0,0.6), var(--prism-rim);
      padding: 26px;
      display: flex;
      flex-direction: column;
      gap: 18px;
    }
    .header-box { display: flex; align-items: center; gap: 14px; }
    .logo-icon {
      width: 48px; height: 48px; border-radius: 14px;
      background: linear-gradient(135deg, #06b6d4, #6366f1);
      display: flex; align-items: center; justify-content: center;
      font-size: 24px; box-shadow: 0 0 24px var(--primary-glow);
    }
    .title-box h1 { font-size: 19px; font-weight: 800; color: #fff; letter-spacing: -0.3px; }
    .title-box p { font-size: 12px; color: var(--text-sub); }
    .badge {
      display: inline-flex; align-items: center; gap: 6px; font-family: monospace; font-size: 11px;
      font-weight: 700; background: rgba(56, 189, 248, 0.15); border: 1px solid var(--primary);
      color: var(--primary); padding: 5px 12px; border-radius: 10px; width: fit-content;
    }
    .dropzone {
      border: 2px dashed rgba(56, 189, 248, 0.4);
      background: rgba(10, 14, 23, 0.6);
      border-radius: 18px; padding: 32px 18px; text-align: center; cursor: pointer;
      transition: all 0.25s; display: flex; flex-direction: column; align-items: center; gap: 10px;
    }
    .dropzone:hover { border-color: var(--primary); background: rgba(56, 189, 248, 0.08); transform: scale(1.01); }
    .file-info {
      display: none; background: rgba(255,255,255,0.06); border: 1px solid var(--card-border);
      border-radius: 12px; padding: 12px 16px; font-size: 12px; color: #fff;
      justify-content: space-between; align-items: center;
    }
    .btn {
      padding: 13px 22px; border-radius: 14px; font-size: 14px; font-weight: 800;
      border: none; cursor: pointer; transition: all 0.2s; display: flex; align-items: center;
      justify-content: center; gap: 8px;
    }
    .btn:active { transform: scale(0.97); }
    .btn-primary {
      background: linear-gradient(135deg, var(--primary), #0284c7);
      color: #000; box-shadow: 0 4px 20px var(--primary-glow);
    }
    .btn-primary:disabled { background: #334155; color: #64748b; cursor: not-allowed; box-shadow: none; }
    .btn-secondary {
      background: rgba(255,255,255,0.08); border: 1px solid var(--card-border);
      color: #fff; font-size: 12px; text-decoration: none; padding: 10px 16px; border-radius: 12px;
      display: flex; align-items: center; justify-content: center; gap: 6px;
    }
    .progress-box {
      display: none; flex-direction: column; gap: 8px; background: rgba(10, 14, 23, 0.6);
      border: 1px solid var(--card-border); border-radius: 14px; padding: 14px 16px;
    }
    .progress-track { width: 100%; height: 10px; background: rgba(255,255,255,0.1); border-radius: 10px; overflow: hidden; }
    .progress-fill { height: 100%; background: linear-gradient(90deg, var(--primary), var(--success)); width: 0%; transition: width 0.2s; }
    .status-msg { display: none; padding: 12px 16px; border-radius: 12px; font-size: 12px; font-weight: 600; line-height: 1.4; }
    .status-success { background: rgba(16, 185, 129, 0.15); border: 1px solid var(--success); color: var(--success); }
    .status-error { background: rgba(255, 59, 48, 0.15); border: 1px solid var(--danger); color: #ff6b8b; }
  </style>
</head>
<body>
  <div class="container">
    <div class="glass-card">
      <div class="header-box">
        <div class="logo-icon">🚀</div>
        <div class="title-box">
          <h1>OTA Firmware Flashing Station</h1>
          <p>Dedicated Port 500 Secure Subsystem</p>
        </div>
      </div>

      <div class="badge">🔒 Dedicated Port 500 &bull; Current FW: v2.0.0-OTA</div>

      <input type="file" id="firmwareFile" accept=".bin" style="display:none;" onchange="handleFileSelected(event)">

      <div class="dropzone" id="dropzone" onclick="document.getElementById('firmwareFile').click()">
        <span style="font-size:36px;">📦</span>
        <div style="font-weight:700; font-size:14px; color:#fff;">Click or Drag & Drop .bin File</div>
        <div style="font-size:12px; color:var(--text-sub);">Select standard compiled binary firmware image (.bin)</div>
      </div>

      <div class="file-info" id="fileInfoBox">
        <span id="fileNameText" style="font-weight:700; overflow:hidden; text-overflow:ellipsis; white-space:nowrap; max-width:70%;">firmware.bin</span>
        <span id="fileSizeText" style="color:var(--primary); font-family:monospace;">0 KB</span>
      </div>

      <div class="progress-box" id="progressContainer">
        <div style="display:flex; justify-content:space-between; font-size:12px;">
          <span id="progressStatusText" style="color:var(--primary); font-weight:700;">Uploading firmware...</span>
          <span id="progressPctText" style="font-weight:800; font-family:monospace; color:var(--primary);">0%</span>
        </div>
        <div class="progress-track">
          <div class="progress-fill" id="progressBar"></div>
        </div>
      </div>

      <div class="status-msg" id="statusBox"></div>

      <button class="btn btn-primary" id="uploadBtn" onclick="startFirmwareUpload()" disabled>
        Flash Firmware to ESP
      </button>

      <a href="http://192.168.4.1/" class="btn-secondary">
        &larr; Return to Main Dashboard (Port 80)
      </a>
    </div>
  </div>

  <script>
    let selectedFile = null;
    function handleFileSelected(e) {
      const file = e.target.files[0];
      if (!file) return;
      if (!file.name.endsWith('.bin')) {
        alert('Please select a valid compiled .bin firmware file.');
        return;
      }
      selectedFile = file;
      document.getElementById('fileNameText').innerText = file.name;
      document.getElementById('fileSizeText').innerText = (file.size / 1024).toFixed(1) + ' KB';
      document.getElementById('fileInfoBox').style.display = 'flex';
      document.getElementById('uploadBtn').disabled = false;
    }

    const dropzone = document.getElementById('dropzone');
    ['dragenter', 'dragover'].forEach(n => dropzone.addEventListener(n, e => { e.preventDefault(); dropzone.style.borderColor = 'var(--primary)'; }));
    ['dragleave', 'drop'].forEach(n => dropzone.addEventListener(n, e => { e.preventDefault(); dropzone.style.borderColor = 'rgba(56, 189, 248, 0.4)'; }));
    dropzone.addEventListener('drop', e => {
      const dt = e.dataTransfer;
      if (dt.files && dt.files[0]) {
        handleFileSelected({ target: { files: dt.files } });
      }
    });

    function startFirmwareUpload() {
      if (!selectedFile) return;
      const btn = document.getElementById('uploadBtn');
      const progressBox = document.getElementById('progressContainer');
      const progressBar = document.getElementById('progressBar');
      const progressPct = document.getElementById('progressPctText');
      const statusBox = document.getElementById('statusBox');

      btn.disabled = true;
      progressBox.style.display = 'flex';
      statusBox.style.display = 'none';

      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/api/ota/upload', true);

      xhr.upload.onprogress = e => {
        if (e.lengthComputable) {
          const pct = Math.round((e.loaded / e.total) * 100);
          progressBar.style.width = pct + '%';
          progressPct.innerText = pct + '%';
        }
      };

      xhr.onload = () => {
        if (xhr.status === 200) {
          statusBox.className = 'status-msg status-success';
          statusBox.innerHTML = '✅ <b>Firmware Flashed Successfully!</b><br>ESP is rebooting now. Reconnect to AP in 10 seconds.';
          statusBox.style.display = 'block';
          setTimeout(() => { window.location.href = 'http://192.168.4.1/'; }, 10000);
        } else {
          statusBox.className = 'status-msg status-error';
          statusBox.innerHTML = '❌ <b>Update Failed:</b> ' + xhr.responseText;
          statusBox.style.display = 'block';
          btn.disabled = false;
        }
      };

      xhr.onerror = () => {
        statusBox.className = 'status-msg status-error';
        statusBox.innerText = 'Network error during upload.';
        statusBox.style.display = 'block';
        btn.disabled = false;
      };

      const formData = new FormData();
      formData.append('firmware', selectedFile, selectedFile.name);
      xhr.send(formData);
    }
  </script>
</body>
</html>
)rawliteral";

// =====================================================================
// MAIN WEB DASHBOARD UI (PORT 80) - APPLE GLASSY PRISM LOOK
// =====================================================================
const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>AURA &bull; ESP8266 Quad-Relay RTOS</title>
  <style>
    :root {
      --bg: #05070e;
      --card-glass: rgba(16, 22, 36, 0.62);
      --card-inner: rgba(10, 14, 24, 0.55);
      --card-border: rgba(255, 255, 255, 0.13);
      --card-border-glow: rgba(56, 189, 248, 0.35);
      --prism-specular: inset 0 1px 1px 0 rgba(255, 255, 255, 0.28);
      --primary: #38bdf8;
      --primary-glow: rgba(56, 189, 248, 0.45);
      --success: #10b981;
      --success-glow: rgba(16, 185, 129, 0.45);
      --danger: #ef4444;
      --danger-glow: rgba(239, 68, 68, 0.45);
      --warning: #f59e0b;
      --purple: #a855f7;
      --purple-glow: rgba(168, 85, 247, 0.45);
      --tg-color: #0ea5e9;
      --text: #f8fafc;
      --text-sub: #94a3b8;
      --text-muted: #64748b;
      --input-bg: rgba(9, 13, 22, 0.75);
      --radius-xl: 26px;
      --radius-lg: 20px;
      --radius-md: 14px;
      --radius-sm: 8px;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }

    body {
      background-color: var(--bg);
      background-image:
        radial-gradient(ellipse 60% 45% at 15% 5%, rgba(99, 102, 241, 0.20), transparent),
        radial-gradient(ellipse 55% 45% at 85% 12%, rgba(56, 189, 248, 0.18), transparent),
        radial-gradient(ellipse 70% 50% at 50% 90%, rgba(16, 185, 129, 0.14), transparent);
      background-attachment: fixed;
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "SF Pro Display", "SF Pro Text", "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      min-height: 100vh;
      padding: 0 0 60px 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      overflow-x: hidden;
      -webkit-font-smoothing: antialiased;
    }

    /* Container */
    .container {
      width: 100%;
      max-width: 680px;
      padding: 16px 14px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    /* Sticky Apple Frosted Navigation Bar */
    .nav-bar {
      width: 100%;
      position: sticky;
      top: 0;
      z-index: 100;
      background: rgba(10, 14, 24, 0.82);
      backdrop-filter: blur(24px) saturate(190%);
      -webkit-backdrop-filter: blur(24px) saturate(190%);
      border-bottom: 1px solid var(--card-border);
      box-shadow: 0 10px 30px rgba(0,0,0,0.5), inset 0 -1px 0 0 rgba(255,255,255,0.06);
      padding: 10px 16px;
      display: flex;
      justify-content: center;
    }
    .nav-inner {
      width: 100%;
      max-width: 680px;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .brand-box {
      display: flex;
      align-items: center;
      gap: 10px;
      cursor: pointer;
    }
    .brand-icon {
      width: 38px;
      height: 38px;
      border-radius: 12px;
      background: linear-gradient(135deg, #00f2fe, #4facfe 50%, #9055ff);
      box-shadow: 0 0 20px var(--primary-glow), inset 0 1px 1px rgba(255,255,255,0.4);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 19px;
      color: #000;
      font-weight: 800;
    }
    .brand-text h1 {
      font-size: 16px;
      font-weight: 800;
      color: #fff;
      letter-spacing: -0.4px;
      line-height: 1.1;
    }
    .brand-text p {
      font-size: 11px;
      color: var(--text-sub);
      font-weight: 500;
    }

    .nav-actions {
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .clock-pill {
      background: rgba(255,255,255,0.06);
      border: 1px solid var(--card-border);
      box-shadow: var(--prism-specular);
      color: var(--primary);
      font-family: monospace;
      font-size: 12px;
      font-weight: 700;
      padding: 6px 12px;
      border-radius: 20px;
      display: flex;
      align-items: center;
      gap: 6px;
      cursor: pointer;
      transition: all 0.2s;
    }
    .clock-pill:active { transform: scale(0.95); background: rgba(56, 189, 248, 0.15); }
    .pulse-dot {
      width: 7px;
      height: 7px;
      border-radius: 50%;
      background: var(--success);
      box-shadow: 0 0 8px var(--success-glow);
    }
    .pulse-dot.unsynced {
      background: var(--warning);
      box-shadow: 0 0 8px rgba(245, 158, 11, 0.5);
    }
    .nav-icon-btn {
      width: 38px;
      height: 38px;
      border-radius: 12px;
      background: rgba(255,255,255,0.06);
      border: 1px solid var(--card-border);
      box-shadow: var(--prism-specular);
      color: #fff;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 18px;
      cursor: pointer;
      transition: all 0.2s;
    }
    .nav-icon-btn:active { transform: scale(0.92); background: rgba(255,255,255,0.14); }

    /* Glass Cards */
    .glass-card {
      background: var(--card-glass);
      backdrop-filter: blur(28px) saturate(200%);
      -webkit-backdrop-filter: blur(28px) saturate(200%);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-lg);
      box-shadow: 0 18px 45px -12px rgba(0, 0, 0, 0.58), var(--prism-specular);
      padding: 18px;
      display: flex;
      flex-direction: column;
      gap: 14px;
      position: relative;
      transition: transform 0.2s, box-shadow 0.2s;
    }

    /* Master Shortcuts Dock */
    .shortcuts-dock {
      background: linear-gradient(135deg, rgba(56, 189, 248, 0.09), rgba(16, 185, 129, 0.07));
      border: 1px solid rgba(56, 189, 248, 0.35);
      border-radius: var(--radius-xl);
      padding: 16px;
      display: flex;
      flex-direction: column;
      gap: 12px;
      box-shadow: 0 20px 50px -10px rgba(0,0,0,0.6), var(--prism-specular);
    }
    .shortcuts-top {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .shortcuts-heading {
      font-size: 13px;
      font-weight: 800;
      color: #fff;
      letter-spacing: -0.2px;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .master-btn-bar {
      display: flex;
      gap: 8px;
    }
    .master-btn {
      font-size: 11px;
      font-weight: 800;
      padding: 6px 14px;
      border-radius: 12px;
      border: none;
      cursor: pointer;
      transition: all 0.2s;
      display: flex;
      align-items: center;
      gap: 5px;
    }
    .master-btn:active { transform: scale(0.95); }
    .btn-all-on {
      background: linear-gradient(135deg, #10b981, #059669);
      color: #000;
      box-shadow: 0 4px 16px var(--success-glow);
    }
    .btn-all-off {
      background: rgba(239, 68, 68, 0.18);
      border: 1px solid var(--danger);
      color: #fca5a5;
    }
    .btn-all-off:active {
      background: var(--danger);
      color: #fff;
    }

    /* 4-Channel Grid */
    .channel-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 10px;
    }
    @media (max-width: 480px) {
      .channel-grid {
        grid-template-columns: repeat(2, 1fr);
      }
    }
    .channel-tile {
      background: rgba(14, 20, 32, 0.65);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-md);
      box-shadow: var(--prism-specular);
      padding: 12px 10px;
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 6px;
      cursor: pointer;
      transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
      user-select: none;
    }
    .channel-tile:active { transform: scale(0.94); }
    .channel-tile.active {
      background: linear-gradient(135deg, rgba(16, 185, 129, 0.25), rgba(56, 189, 248, 0.18));
      border-color: var(--success);
      box-shadow: 0 0 20px var(--success-glow), var(--prism-specular);
    }
    .tile-badge {
      font-size: 11px;
      font-weight: 800;
      padding: 3px 8px;
      border-radius: 8px;
      font-family: monospace;
      background: rgba(255,255,255,0.08);
      color: var(--text-sub);
    }
    .channel-tile.active .tile-badge {
      background: var(--success);
      color: #000;
    }
    .tile-name {
      font-size: 12px;
      font-weight: 700;
      color: #fff;
      text-align: center;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
      width: 100%;
    }
    .tile-power {
      font-size: 10px;
      color: var(--text-sub);
      font-family: monospace;
    }
    .tile-state-pill {
      font-size: 10px;
      font-weight: 800;
      letter-spacing: 0.5px;
      padding: 2px 8px;
      border-radius: 6px;
      background: rgba(255,255,255,0.06);
      color: var(--text-muted);
    }
    .channel-tile.active .tile-state-pill {
      background: rgba(16, 185, 129, 0.25);
      border: 1px solid var(--success);
      color: var(--success);
    }

    /* Auto-Sleep / Power Status Banner */
    .sleep-banner {
      background: linear-gradient(135deg, rgba(245, 158, 11, 0.12), rgba(56, 189, 248, 0.08));
      border: 1px solid rgba(245, 158, 11, 0.35);
      border-radius: var(--radius-lg);
      padding: 12px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      box-shadow: var(--prism-specular);
    }
    .sleep-banner-left {
      display: flex;
      flex-direction: column;
      gap: 2px;
    }
    .sleep-banner-title {
      font-size: 12px;
      font-weight: 800;
      color: #fff;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .sleep-banner-desc {
      font-size: 11px;
      color: var(--text-sub);
    }
    .sleep-banner-timer {
      font-family: monospace;
      font-size: 14px;
      font-weight: 800;
      color: var(--warning);
    }
    .sleep-wake-btn {
      background: rgba(245, 158, 11, 0.2);
      border: 1px solid var(--warning);
      color: #fde68a;
      font-size: 11px;
      font-weight: 700;
      padding: 6px 12px;
      border-radius: 10px;
      cursor: pointer;
      transition: all 0.2s;
    }
    .sleep-wake-btn:active { transform: scale(0.94); background: var(--warning); color: #000; }

    /* Relay Detailed Cards */
    .relay-card {
      background: var(--card-glass);
      backdrop-filter: blur(28px) saturate(200%);
      -webkit-backdrop-filter: blur(28px) saturate(200%);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-lg);
      box-shadow: 0 16px 40px -10px rgba(0,0,0,0.55), var(--prism-specular);
      padding: 18px;
      display: flex;
      flex-direction: column;
      gap: 14px;
      transition: border-color 0.25s, box-shadow 0.25s;
    }
    .relay-card.active {
      border-color: rgba(16, 185, 129, 0.45);
      box-shadow: 0 16px 45px -10px rgba(16, 185, 129, 0.2), var(--prism-specular);
    }
    .relay-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      cursor: pointer;
      user-select: none;
    }
    .relay-info-box {
      display: flex;
      align-items: center;
      gap: 12px;
    }
    .relay-badge {
      width: 44px;
      height: 44px;
      border-radius: 14px;
      background: rgba(255, 255, 255, 0.08);
      border: 1px solid var(--card-border);
      box-shadow: var(--prism-specular);
      display: flex;
      align-items: center;
      justify-content: center;
      font-weight: 800;
      font-size: 16px;
      color: var(--primary);
      transition: all 0.25s;
    }
    .relay-card.active .relay-badge {
      background: linear-gradient(135deg, #10b981, #059669);
      color: #000;
      box-shadow: 0 0 20px var(--success-glow);
    }
    .relay-meta h2 {
      font-size: 15px;
      font-weight: 800;
      color: #fff;
      letter-spacing: -0.2px;
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .relay-meta p {
      font-size: 11px;
      color: var(--text-sub);
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .countdown-pill {
      background: rgba(56, 189, 248, 0.18);
      border: 1px solid var(--primary);
      color: var(--primary);
      font-family: monospace;
      font-size: 11px;
      font-weight: 800;
      padding: 2px 8px;
      border-radius: 6px;
      animation: pulseGlow 1.8s infinite;
    }
    @keyframes pulseGlow {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.6; }
    }

    /* Apple Glass Toggle Capsule */
    .apple-toggle {
      width: 52px;
      height: 30px;
      border-radius: 30px;
      background: rgba(255, 255, 255, 0.12);
      border: 1px solid rgba(255, 255, 255, 0.2);
      box-shadow: inset 0 2px 4px rgba(0,0,0,0.3);
      position: relative;
      cursor: pointer;
      transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
    }
    .apple-toggle::after {
      content: '';
      position: absolute;
      top: 3px;
      left: 3px;
      width: 22px;
      height: 22px;
      border-radius: 50%;
      background: #ffffff;
      box-shadow: 0 2px 6px rgba(0,0,0,0.4);
      transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
    }
    .apple-toggle.active {
      background: linear-gradient(135deg, #10b981, #06b6d4);
      border-color: rgba(16, 185, 129, 0.6);
      box-shadow: 0 0 18px var(--success-glow), inset 0 1px 1px rgba(255,255,255,0.4);
    }
    .apple-toggle.active::after {
      transform: translateX(22px);
    }

    /* Segmented Tab Bar */
    .tab-bar {
      display: flex;
      background: rgba(10, 14, 24, 0.7);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-md);
      padding: 4px;
      gap: 4px;
      overflow-x: auto;
    }
    .tab-btn {
      flex: 1;
      padding: 7px 10px;
      font-size: 11px;
      font-weight: 700;
      color: var(--text-sub);
      background: transparent;
      border: none;
      border-radius: 10px;
      cursor: pointer;
      transition: all 0.2s;
      white-space: nowrap;
      text-align: center;
    }
    .tab-btn.active {
      background: rgba(255, 255, 255, 0.14);
      color: #fff;
      box-shadow: var(--prism-specular), 0 2px 8px rgba(0,0,0,0.3);
    }

    /* Tab Panes */
    .tab-content {
      display: none;
      flex-direction: column;
      gap: 12px;
      padding-top: 4px;
    }
    .tab-content.active {
      display: flex;
    }

    /* Inputs, Chips, Controls */
    .form-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }
    @media (max-width: 480px) {
      .form-row { grid-template-columns: 1fr; }
    }
    .input-group {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }
    .input-label {
      font-size: 11px;
      font-weight: 600;
      color: var(--text-sub);
    }
    .glass-input {
      background: var(--input-bg);
      border: 1px solid var(--card-border);
      color: #fff;
      padding: 9px 12px;
      border-radius: 10px;
      font-size: 13px;
      outline: none;
      transition: border-color 0.2s;
    }
    .glass-input:focus {
      border-color: var(--primary);
    }
    .chips-bar {
      display: flex;
      gap: 6px;
      flex-wrap: wrap;
    }
    .chip {
      background: rgba(255,255,255,0.06);
      border: 1px solid var(--card-border);
      color: var(--text-sub);
      font-size: 11px;
      font-weight: 700;
      padding: 5px 10px;
      border-radius: 8px;
      cursor: pointer;
      transition: all 0.2s;
    }
    .chip:hover, .chip:active {
      background: rgba(56, 189, 248, 0.15);
      border-color: var(--primary);
      color: var(--primary);
    }
    .btn-action-group {
      display: flex;
      gap: 8px;
    }
    .btn-glass {
      padding: 9px 16px;
      border-radius: 12px;
      font-size: 12px;
      font-weight: 800;
      border: none;
      cursor: pointer;
      transition: all 0.2s;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
    }
    .btn-glass:active { transform: scale(0.96); }
    .btn-primary-glass {
      background: linear-gradient(135deg, var(--primary), #0284c7);
      color: #000;
      box-shadow: 0 4px 16px var(--primary-glow);
    }
    .btn-danger-glass {
      background: rgba(239, 68, 68, 0.15);
      border: 1px solid var(--danger);
      color: #fca5a5;
    }

    /* Days of Week Pills */
    .days-grid {
      display: flex;
      gap: 4px;
      justify-content: space-between;
    }
    .day-pill {
      flex: 1;
      padding: 6px 2px;
      font-size: 11px;
      font-weight: 700;
      border-radius: 8px;
      background: rgba(255,255,255,0.06);
      border: 1px solid var(--card-border);
      color: var(--text-muted);
      cursor: pointer;
      text-align: center;
      transition: all 0.2s;
    }
    .day-pill.active {
      background: rgba(56, 189, 248, 0.2);
      border-color: var(--primary);
      color: var(--primary);
      font-weight: 800;
    }

    /* Diagnostics & Telemetry */
    .metrics-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 8px;
    }
    @media (max-width: 480px) {
      .metrics-grid { grid-template-columns: repeat(2, 1fr); }
    }
    .metric-box {
      background: rgba(10, 14, 24, 0.55);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 10px 12px;
      display: flex;
      flex-direction: column;
      gap: 2px;
    }
    .metric-label {
      font-size: 10px;
      font-weight: 600;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    .metric-val {
      font-size: 13px;
      font-weight: 800;
      color: #fff;
      font-family: monospace;
    }

    /* Event Log Terminal */
    .log-terminal {
      background: #060911;
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 12px;
      padding: 10px 12px;
      max-height: 140px;
      overflow-y: auto;
      font-family: monospace;
      font-size: 11px;
      display: flex;
      flex-direction: column;
      gap: 4px;
    }
    .log-line {
      display: flex;
      gap: 8px;
      line-height: 1.3;
    }
    .log-time {
      color: var(--primary);
      flex-shrink: 0;
    }
    .log-msg {
      color: var(--text-sub);
    }

    /* Sidenav Overlay & Drawer */
    .drawer-overlay {
      position: fixed;
      top: 0; left: 0; width: 100vw; height: 100vh;
      background: rgba(0, 0, 0, 0.65);
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
      z-index: 200;
      opacity: 0; pointer-events: none;
      transition: opacity 0.3s;
    }
    .drawer-overlay.open { opacity: 1; pointer-events: auto; }
    .drawer {
      position: fixed;
      top: 0; right: -360px; width: 340px; height: 100vh;
      background: rgba(12, 17, 29, 0.94);
      backdrop-filter: blur(28px) saturate(200%);
      -webkit-backdrop-filter: blur(28px) saturate(200%);
      border-left: 1px solid var(--card-border);
      box-shadow: -10px 0 40px rgba(0,0,0,0.6);
      z-index: 201;
      padding: 24px 20px;
      display: flex;
      flex-direction: column;
      gap: 20px;
      transition: right 0.3s cubic-bezier(0.4, 0, 0.2, 1);
      overflow-y: auto;
    }
    .drawer.open { right: 0; }
    .drawer-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      border-bottom: 1px solid var(--card-border);
      padding-bottom: 14px;
    }
    .drawer-title { font-size: 16px; font-weight: 800; color: #fff; }
    .drawer-close {
      width: 32px; height: 32px; border-radius: 10px;
      background: rgba(255,255,255,0.08); border: 1px solid var(--card-border);
      color: #fff; display: flex; align-items: center; justify-content: center;
      cursor: pointer;
    }
    .drawer-section {
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .drawer-section-title {
      font-size: 11px;
      font-weight: 800;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.8px;
    }

    /* Toast */
    .toast {
      position: fixed;
      bottom: 24px;
      background: rgba(16, 22, 36, 0.92);
      backdrop-filter: blur(18px);
      -webkit-backdrop-filter: blur(18px);
      border: 1px solid var(--primary);
      box-shadow: 0 10px 30px rgba(0,0,0,0.6), 0 0 15px var(--primary-glow);
      color: #fff;
      font-size: 13px;
      font-weight: 700;
      padding: 10px 20px;
      border-radius: 20px;
      z-index: 300;
      opacity: 0;
      transform: translateY(20px);
      transition: all 0.3s;
      pointer-events: none;
    }
    .toast.show {
      opacity: 1;
      transform: translateY(0);
    }
  </style>
</head>
<body>

  <!-- Sticky Top Frosted Bar -->
  <header class="nav-bar">
    <div class="nav-inner">
      <div class="brand-box" onclick="window.scrollTo({top:0, behavior:'smooth'})">
        <div class="brand-icon">⚡</div>
        <div class="brand-text">
          <h1>AURA RTOS</h1>
          <p>Quad Relay Engine &bull; ESP8266</p>
        </div>
      </div>
      <div class="nav-actions">
        <div class="clock-pill" id="navClockPill" onclick="syncPhoneTime()" title="Click to synchronize with Phone Clock">
          <div class="pulse-dot" id="navSyncDot"></div>
          <span id="navClockText">00:00:00</span>
        </div>
        <div class="nav-icon-btn" onclick="toggleDrawer(true)" title="Settings & Telegram">
          ⚙️
        </div>
      </div>
    </div>
  </header>

  <div class="container">

    <!-- Master Shortcuts Glass Dock (Screen Shortcut) -->
    <section class="shortcuts-dock">
      <div class="shortcuts-top">
        <div class="shortcuts-heading">
          <span>⚡</span> Quick Relay Shortcuts
        </div>
        <div class="master-btn-bar">
          <button class="master-btn btn-all-on" onclick="setMasterAll(true)">⚡ ALL ON</button>
          <button class="master-btn btn-all-off" onclick="setMasterAll(false)">🚨 ALL OFF</button>
        </div>
      </div>

      <div class="channel-grid">
        <div class="channel-tile" id="tile1" onclick="quickToggleRelay(1)">
          <div class="tile-badge">R1</div>
          <div class="tile-name" id="tileName1">Relay 1</div>
          <div class="tile-power" id="tilePower1">100W</div>
          <div class="tile-state-pill" id="tileState1">OFF</div>
        </div>
        <div class="channel-tile" id="tile2" onclick="quickToggleRelay(2)">
          <div class="tile-badge">R2</div>
          <div class="tile-name" id="tileName2">Relay 2</div>
          <div class="tile-power" id="tilePower2">100W</div>
          <div class="tile-state-pill" id="tileState2">OFF</div>
        </div>
        <div class="channel-tile" id="tile3" onclick="quickToggleRelay(3)">
          <div class="tile-badge">R3</div>
          <div class="tile-name" id="tileName3">Relay 3</div>
          <div class="tile-power" id="tilePower3">100W</div>
          <div class="tile-state-pill" id="tileState3">OFF</div>
        </div>
        <div class="channel-tile" id="tile4" onclick="quickToggleRelay(4)">
          <div class="tile-badge">R4</div>
          <div class="tile-name" id="tileName4">Relay 4</div>
          <div class="tile-power" id="tilePower4">100W</div>
          <div class="tile-state-pill" id="tileState4">OFF</div>
        </div>
      </div>
    </section>

    <!-- Auto-Sleep / Power Status Banner -->
    <div class="sleep-banner" id="sleepBanner">
      <div class="sleep-banner-left">
        <div class="sleep-banner-title">
          <span>🌱</span> <span id="sleepBannerStatus">Power Saver: Always-On</span>
        </div>
        <div class="sleep-banner-desc" id="sleepBannerDesc">System stays awake permanently. No sleep interruption.</div>
      </div>
      <div style="display:flex; align-items:center; gap:8px;">
        <span class="sleep-banner-timer" id="sleepBannerTimer"></span>
        <button class="sleep-wake-btn" id="sleepWakeBtn" style="display:none;" onclick="wakeESPNow()">Wake Now</button>
      </div>
    </div>

    <!-- 4 Individual Relay Channel Cards -->
    <div id="relayCardsContainer" style="display:flex; flex-direction:column; gap:16px;"></div>

    <!-- System Diagnostics & Telemetry Glass Card -->
    <div class="glass-card">
      <div style="display:flex; justify-content:space-between; align-items:center;">
        <div style="font-size:13px; font-weight:800; display:flex; align-items:center; gap:6px;">
          <span>📊</span> System Diagnostics & Energy
        </div>
        <div style="font-size:11px; font-family:monospace; color:var(--success); font-weight:700;" id="totalEnergyKwh">
          0.000 kWh Total
        </div>
      </div>

      <div class="metrics-grid">
        <div class="metric-box">
          <span class="metric-label">Uptime</span>
          <span class="metric-val" id="metricUptime">0h 0m</span>
        </div>
        <div class="metric-box">
          <span class="metric-label">Free RAM</span>
          <span class="metric-val" id="metricHeap">0 KB</span>
        </div>
        <div class="metric-box">
          <span class="metric-label">Temp</span>
          <span class="metric-val" id="metricTemp">32.0 °C</span>
        </div>
        <div class="metric-box">
          <span class="metric-label">AP Clients</span>
          <span class="metric-val" id="metricClients">0 connected</span>
        </div>
        <div class="metric-box">
          <span class="metric-label">Relay Ticks</span>
          <span class="metric-val" id="metricRelayTicks">0</span>
        </div>
        <div class="metric-box">
          <span class="metric-label">Power Ticks</span>
          <span class="metric-val" id="metricPowerTicks">0</span>
        </div>
      </div>

      <div style="display:flex; flex-direction:column; gap:6px;">
        <span style="font-size:11px; font-weight:700; color:var(--text-sub);">Real-Time Event Stream</span>
        <div class="log-terminal" id="eventLogBox">
          <div class="log-line"><span class="log-time">[BOOT]</span><span class="log-msg">System initialized</span></div>
        </div>
      </div>
    </div>

  </div>

  <!-- Sidenav Settings Drawer -->
  <div class="drawer-overlay" id="drawerOverlay" onclick="toggleDrawer(false)"></div>
  <aside class="drawer" id="drawer">
    <div class="drawer-header">
      <div class="drawer-title">Settings & System</div>
      <div class="drawer-close" onclick="toggleDrawer(false)">&times;</div>
    </div>

    <!-- Power Saver Management Section -->
    <div class="drawer-section">
      <div class="drawer-section-title">Power Management</div>
      <div class="input-group">
        <label class="input-label">Operating Mode</label>
        <select class="glass-input" id="cfgPowerMode" onchange="submitPowerConfig()">
          <option value="stay_on">Permanent Always-On (Never Sleep)</option>
          <option value="low_power">Low Power Auto-Sleep (AP Duty Cycle)</option>
        </select>
      </div>
      <div class="form-row">
        <div class="input-group">
          <label class="input-label">Wake Window (min)</label>
          <input type="number" class="glass-input" id="cfgWakeMin" min="1" max="60" value="2">
        </div>
        <div class="input-group">
          <label class="input-label">Sleep Interval (min)</label>
          <input type="number" class="glass-input" id="cfgSleepMin" min="1" max="120" value="5">
        </div>
      </div>
      <button class="btn-glass btn-primary-glass" onclick="submitPowerConfig()">Save Power Config</button>
    </div>

    <!-- Telegram Notifications Section -->
    <div class="drawer-section">
      <div class="drawer-section-title">Telegram Remote Control</div>
      <div class="input-group">
        <label class="input-label">Enable Telegram Bot</label>
        <select class="glass-input" id="cfgTgEnable">
          <option value="0">Disabled</option>
          <option value="1">Enabled</option>
        </select>
      </div>
      <div class="input-group">
        <label class="input-label">Bot API Token</label>
        <input type="password" class="glass-input" id="cfgTgToken" placeholder="123456789:ABCdef...">
      </div>
      <div class="input-group">
        <label class="input-label">Target Chat ID</label>
        <input type="text" class="glass-input" id="cfgTgChat" placeholder="e.g. 987654321">
      </div>
      <div class="form-row">
        <div class="input-group">
          <label class="input-label">Home Wi-Fi SSID</label>
          <input type="text" class="glass-input" id="cfgTgSsid" placeholder="Home WiFi">
        </div>
        <div class="input-group">
          <label class="input-label">Wi-Fi Password</label>
          <input type="password" class="glass-input" id="cfgTgPass" placeholder="Password">
        </div>
      </div>
      <div style="display:flex; flex-direction:column; gap:6px; font-size:12px; color:var(--text-sub);">
        <label><input type="checkbox" id="cfgTgNTog" checked> Notify on Relay Toggle</label>
        <label><input type="checkbox" id="cfgTgNTim" checked> Notify on Timer Finish</label>
        <label><input type="checkbox" id="cfgTgNSch" checked> Notify on Schedule Trigger</label>
      </div>
      <div class="btn-action-group">
        <button class="btn-glass btn-primary-glass" style="flex:1;" onclick="saveTelegramConfig()">Save Telegram</button>
        <button class="btn-glass btn-secondary-glass" onclick="testTelegram()">Test Bot</button>
      </div>
    </div>

    <!-- OTA Firmware Flashing Section -->
    <div class="drawer-section">
      <div class="drawer-section-title">Firmware & OTA Update</div>
      <input type="file" id="drawerOtaFile" accept=".bin" style="display:none;" onchange="handleDrawerOtaFile(event)">
      <button class="btn-glass" style="background:rgba(255,255,255,0.08); width:100%;" onclick="document.getElementById('drawerOtaFile').click()">
        📦 Select .bin Firmware File
      </button>
      <div id="drawerOtaInfo" style="display:none; font-size:11px; color:var(--primary); font-family:monospace; justify-content:space-between;">
        <span id="drawerOtaName"></span>
        <span id="drawerOtaSize"></span>
      </div>
      <button class="btn-glass btn-primary-glass" id="drawerOtaBtn" style="display:none;" onclick="uploadDrawerOta()">
        Flash Firmware (Port 80)
      </button>

      <div style="border-top:1px solid var(--card-border); padding-top:10px; display:flex; flex-direction:column; gap:6px;">
        <span class="input-label">Dedicated Security Server</span>
        <div style="display:flex; justify-content:space-between; align-items:center;">
          <span style="font-size:12px; color:#fff;">Port 500 Server</span>
          <button class="btn-glass" id="port500ToggleBtn" onclick="togglePort500()">Enable</button>
        </div>
        <a href="http://192.168.4.1:500/" id="port500Link" target="_blank" style="display:none; font-size:11px; color:var(--primary);">
          &rarr; Open Port 500 Portal in New Tab
        </a>
      </div>
    </div>

  </aside>

  <!-- Toast Notification -->
  <div class="toast" id="toast">Settings Saved</div>

  <script>
    const relayData = {};
    const scheduleDays = { 1: 0xFF, 2: 0xFF, 3: 0xFF, 4: 0xFF };
    const dayNames = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
    const pinLabels = { 1: 'D1 / GPIO 5', 2: 'D2 / GPIO 4', 3: 'D5 / GPIO 14', 4: 'D6 / GPIO 12' };

    function showToast(msg) {
      const t = document.getElementById('toast');
      t.innerText = msg;
      t.classList.add('show');
      setTimeout(() => t.classList.remove('show'), 2200);
    }

    function toggleDrawer(open) {
      document.getElementById('drawer').classList.toggle('open', open);
      document.getElementById('drawerOverlay').classList.toggle('open', open);
    }

    // Build the 4 Relay Channel Cards DOM dynamically
    function buildRelayCards() {
      const container = document.getElementById('relayCardsContainer');
      let html = '';

      for (let ch = 1; ch <= 4; ch++) {
        html += `
        <div class="relay-card" id="relayCard${ch}">
          <div class="relay-header" onclick="toggleRelayAccordion(${ch})">
            <div class="relay-info-box">
              <div class="relay-badge" id="badgeCh${ch}">R${ch}</div>
              <div class="relay-meta">
                <h2>
                  <span id="rNameText${ch}">Relay ${ch}</span>
                  <span class="countdown-pill" id="rCountdownPill${ch}" style="display:none;">00:00</span>
                </h2>
                <p>
                  <span>${pinLabels[ch]}</span> &bull;
                  <span id="rRatedWatts${ch}">100W</span> &bull;
                  <span id="rStateLabel${ch}">OFF</span>
                </p>
              </div>
            </div>
            <div class="apple-toggle" id="appleToggle${ch}" onclick="event.stopPropagation(); quickToggleRelay(${ch})"></div>
          </div>

          <!-- Segmented Tab Bar -->
          <div class="tab-bar">
            <button class="tab-btn active" id="tabBtnTimer${ch}" onclick="switchRelayTab(${ch}, 'timer')">⏱ Timer</button>
            <button class="tab-btn" id="tabBtnCycle${ch}" onclick="switchRelayTab(${ch}, 'cycle')">🔁 Cycle</button>
            <button class="tab-btn" id="tabBtnSched${ch}" onclick="switchRelayTab(${ch}, 'sched')">📅 Schedule</button>
            <button class="tab-btn" id="tabBtnSettings${ch}" onclick="switchRelayTab(${ch}, 'settings')">⚡ Settings</button>
          </div>

          <!-- Timer Pane -->
          <div class="tab-content active" id="tabPaneTimer${ch}">
            <div class="chips-bar">
              <span class="chip" onclick="setTimerPreset(${ch}, 60)">+1m</span>
              <span class="chip" onclick="setTimerPreset(${ch}, 300)">+5m</span>
              <span class="chip" onclick="setTimerPreset(${ch}, 900)">+15m</span>
              <span class="chip" onclick="setTimerPreset(${ch}, 1800)">+30m</span>
              <span class="chip" onclick="setTimerPreset(${ch}, 3600)">+1h</span>
            </div>
            <div class="form-row">
              <div class="input-group">
                <label class="input-label">Start Delay (Seconds)</label>
                <input type="number" class="glass-input" id="rTimerDelay${ch}" min="0" value="0">
              </div>
              <div class="input-group">
                <label class="input-label">ON Duration (Seconds)</label>
                <input type="number" class="glass-input" id="rTimerDuration${ch}" min="1" value="300">
              </div>
            </div>
            <div class="btn-action-group">
              <button class="btn-glass btn-primary-glass" style="flex:1;" onclick="startTimer(${ch})">Start Countdown</button>
              <button class="btn-glass btn-danger-glass" onclick="cancelTimer(${ch})">Cancel</button>
            </div>
          </div>

          <!-- Cycle Pane -->
          <div class="tab-content" id="tabPaneCycle${ch}">
            <div class="form-row">
              <div class="input-group">
                <label class="input-label">ON Period (Seconds)</label>
                <input type="number" class="glass-input" id="rCycleOn${ch}" min="1" value="60">
              </div>
              <div class="input-group">
                <label class="input-label">OFF Period (Seconds)</label>
                <input type="number" class="glass-input" id="rCycleOff${ch}" min="1" value="60">
              </div>
            </div>
            <div class="input-group">
              <label class="input-label">Repeat Count (0 = Infinite Loop)</label>
              <input type="number" class="glass-input" id="rCycleCount${ch}" min="0" value="0">
            </div>
            <div class="btn-action-group">
              <button class="btn-glass btn-primary-glass" style="flex:1;" onclick="startCycle(${ch})">Start Cycle Loop</button>
              <button class="btn-glass btn-danger-glass" onclick="cancelCycle(${ch})">Stop</button>
            </div>
          </div>

          <!-- Schedule Pane -->
          <div class="tab-content" id="tabPaneSched${ch}">
            <div class="form-row">
              <div class="input-group">
                <label class="input-label">Start Time (24h)</label>
                <input type="time" class="glass-input" id="rSchedStart${ch}" value="08:00">
              </div>
              <div class="input-group">
                <label class="input-label">End Time (24h)</label>
                <input type="time" class="glass-input" id="rSchedEnd${ch}" value="18:00">
              </div>
            </div>
            <div class="input-group">
              <label class="input-label">Active Days</label>
              <div class="days-grid" id="daysGrid${ch}">
                ${dayNames.map((d, idx) => `<div class="day-pill active" id="dayPill_${ch}_${idx}" onclick="toggleDayPill(${ch}, ${idx})">${d}</div>`).join('')}
              </div>
            </div>
            <div class="btn-action-group">
              <button class="btn-glass btn-primary-glass" style="flex:1;" onclick="saveSchedule(${ch})">Save Schedule</button>
              <button class="btn-glass btn-danger-glass" onclick="cancelSchedule(${ch})">Disable</button>
            </div>
          </div>

          <!-- Settings Pane -->
          <div class="tab-content" id="tabPaneSettings${ch}">
            <div class="form-row">
              <div class="input-group">
                <label class="input-label">Channel Friendly Name</label>
                <input type="text" class="glass-input" id="rCfgName${ch}" value="Relay ${ch}">
              </div>
              <div class="input-group">
                <label class="input-label">Load Power (Watts)</label>
                <input type="number" class="glass-input" id="rCfgWatts${ch}" min="1" value="100">
              </div>
            </div>
            <div class="form-row">
              <div class="input-group">
                <label class="input-label">Relay Trigger Logic</label>
                <select class="glass-input" id="rCfgActLow${ch}">
                  <option value="1">Active LOW (Standard Module)</option>
                  <option value="0">Active HIGH</option>
                </select>
              </div>
              <div class="input-group">
                <label class="input-label">Power-On Default</label>
                <select class="glass-input" id="rCfgPwrOn${ch}">
                  <option value="0">Always Boot OFF (Safest)</option>
                  <option value="1">Always Boot ON</option>
                  <option value="2">Restore Last State</option>
                </select>
              </div>
            </div>
            <button class="btn-glass btn-primary-glass" onclick="saveRelaySettings(${ch})">Save Channel Settings</button>
          </div>

        </div>`;
      }
      container.innerHTML = html;
    }

    function toggleRelayAccordion(ch) {
      // Allows smooth view expansion
    }

    function switchRelayTab(ch, tab) {
      const card = document.getElementById(`relayCard${ch}`);
      card.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      card.querySelectorAll('.tab-content').forEach(p => p.classList.remove('active'));

      if (tab === 'timer') {
        document.getElementById(`tabBtnTimer${ch}`).classList.add('active');
        document.getElementById(`tabPaneTimer${ch}`).classList.add('active');
      } else if (tab === 'cycle') {
        document.getElementById(`tabBtnCycle${ch}`).classList.add('active');
        document.getElementById(`tabPaneCycle${ch}`).classList.add('active');
      } else if (tab === 'sched') {
        document.getElementById(`tabBtnSched${ch}`).classList.add('active');
        document.getElementById(`tabPaneSched${ch}`).classList.add('active');
      } else if (tab === 'settings') {
        document.getElementById(`tabBtnSettings${ch}`).classList.add('active');
        document.getElementById(`tabPaneSettings${ch}`).classList.add('active');
      }
    }

    function toggleDayPill(ch, dayIdx) {
      const pill = document.getElementById(`dayPill_${ch}_${dayIdx}`);
      const bit = 1 << dayIdx;
      scheduleDays[ch] ^= bit;
      pill.classList.toggle('active', (scheduleDays[ch] & bit) !== 0);
    }

    function setTimerPreset(ch, secs) {
      document.getElementById(`rTimerDuration${ch}`).value = secs;
      showToast(`+${secs >= 60 ? secs/60 + 'm' : secs + 's'} selected for R${ch}`);
    }

    // API Calls
    async function quickToggleRelay(ch) {
      const currentState = relayData[`r${ch}`] ? relayData[`r${ch}`].state : false;
      const targetState = !currentState;

      // Optimistic UI update (0ms lag feel)
      updateChannelVisuals(ch, targetState);

      try {
        await fetch('/api/relay', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&state=${targetState ? 1 : 0}`
        });
        showToast(`Relay ${ch} switched ${targetState ? 'ON' : 'OFF'}`);
      } catch (e) {
        console.error(e);
      }
    }

    async function setMasterAll(on) {
      // Optimistic visual update
      for (let ch = 1; ch <= 4; ch++) updateChannelVisuals(ch, on);
      try {
        await fetch(on ? '/api/all_on' : '/api/all_off', { method: 'POST' });
        showToast(on ? '⚡ All 4 Relays Switched ON' : '🚨 All Relays Forced OFF');
      } catch (e) {
        console.error(e);
      }
    }

    async function startTimer(ch) {
      const delay = parseInt(document.getElementById(`rTimerDelay${ch}`).value) || 0;
      const duration = parseInt(document.getElementById(`rTimerDuration${ch}`).value) || 60;
      try {
        await fetch('/api/timer', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&start_delay=${delay}&duration=${duration}&enable=1`
        });
        showToast(`Timer started for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function cancelTimer(ch) {
      try {
        await fetch('/api/timer', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&enable=0`
        });
        showToast(`Timer cancelled for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function startCycle(ch) {
      const onSec = parseInt(document.getElementById(`rCycleOn${ch}`).value) || 60;
      const offSec = parseInt(document.getElementById(`rCycleOff${ch}`).value) || 60;
      const cycles = parseInt(document.getElementById(`rCycleCount${ch}`).value) || 0;
      try {
        await fetch('/api/cycle', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&on_sec=${onSec}&off_sec=${offSec}&cycles=${cycles}&enable=1`
        });
        showToast(`Cycle loop started for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function cancelCycle(ch) {
      try {
        await fetch('/api/cycle', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&enable=0`
        });
        showToast(`Cycle loop stopped for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function saveSchedule(ch) {
      const startTime = document.getElementById(`rSchedStart${ch}`).value || "08:00";
      const endTime = document.getElementById(`rSchedEnd${ch}`).value || "18:00";
      const [sh, sm] = startTime.split(':').map(Number);
      const [eh, em] = endTime.split(':').map(Number);
      const days = scheduleDays[ch];

      try {
        await fetch('/api/schedule', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&start_h=${sh}&start_m=${sm}&end_h=${eh}&end_m=${em}&days=${days}&enable=1`
        });
        showToast(`Schedule saved for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function cancelSchedule(ch) {
      try {
        await fetch('/api/schedule', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `id=${ch}&enable=0`
        });
        showToast(`Schedule disabled for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function saveRelaySettings(ch) {
      const name = encodeURIComponent(document.getElementById(`rCfgName${ch}`).value.trim());
      const watts = parseFloat(document.getElementById(`rCfgWatts${ch}`).value) || 100.0;
      const actLow = document.getElementById(`rCfgActLow${ch}`).value;
      const pwrOn = document.getElementById(`rCfgPwrOn${ch}`).value;

      try {
        await fetch('/api/settings', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `r${ch}_name=${name}&r${ch}_watts=${watts}&r${ch}_actlow=${actLow}&r${ch}_pwron=${pwrOn}`
        });
        showToast(`Settings saved for Relay ${ch}`);
      } catch (e) { console.error(e); }
    }

    async function syncPhoneTime() {
      const nowEpoch = Math.floor(Date.now() / 1000);
      const tzOffsetMinutes = -new Date().getTimezoneOffset(); // e.g. +330 for IST (+5:30)

      try {
        await fetch('/api/time/sync', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `epoch=${nowEpoch}&tz_offset=${tzOffsetMinutes}`
        });
        showToast('Phone Time Synchronized!');
      } catch (e) { console.error(e); }
    }

    async function submitPowerConfig() {
      const mode = document.getElementById('cfgPowerMode').value;
      const sleepMin = parseInt(document.getElementById('cfgSleepMin').value) || 5;
      const wakeMin = parseInt(document.getElementById('cfgWakeMin').value) || 2;
      const stayOn = (mode === 'stay_on');

      try {
        await fetch('/api/power', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `stay_on=${stayOn ? 1 : 0}&sleep_min=${sleepMin}&wake_min=${wakeMin}`
        });
        showToast(stayOn ? 'Permanent Stay-On Activated' : 'Auto-Sleep Duty Cycle Activated');
      } catch (e) { console.error(e); }
    }

    async function wakeESPNow() {
      try {
        await fetch('/api/relay', { method: 'POST', headers: {'Content-Type': 'application/x-www-form-urlencoded'}, body: 'id=1' });
        showToast('ESP Raised / Awake!');
      } catch (e) { console.error(e); }
    }

    async function saveTelegramConfig() {
      const en = document.getElementById('cfgTgEnable').value;
      const tok = encodeURIComponent(document.getElementById('cfgTgToken').value.trim());
      const chat = encodeURIComponent(document.getElementById('cfgTgChat').value.trim());
      const ssid = encodeURIComponent(document.getElementById('cfgTgSsid').value.trim());
      const pass = encodeURIComponent(document.getElementById('cfgTgPass').value);
      const nTog = document.getElementById('cfgTgNTog').checked ? 1 : 0;
      const nTim = document.getElementById('cfgTgNTim').checked ? 1 : 0;
      const nSch = document.getElementById('cfgTgNSch').checked ? 1 : 0;

      try {
        await fetch('/api/telegram/config', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `enable=${en}&token=${tok}&chat_id=${chat}&sta_ssid=${ssid}&sta_pass=${pass}&n_tog=${nTog}&n_tim=${nTim}&n_sch=${nSch}`
        });
        showToast('Telegram Config Saved');
      } catch (e) { console.error(e); }
    }

    async function testTelegram() {
      showToast('Sending test message...');
      try {
        const res = await fetch('/api/telegram/test', { method: 'POST' });
        if (res.ok) {
          showToast('✅ Test Message Sent to Telegram!');
        } else {
          showToast('❌ Send Failed! Check Wi-Fi & Bot token.');
        }
      } catch (e) {
        showToast('❌ Network error during test.');
      }
    }

    async function togglePort500() {
      const isEnabled = document.getElementById('port500ToggleBtn').innerText === 'Disable';
      const newState = !isEnabled;
      try {
        await fetch('/api/ota/port500', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `enable=${newState ? 1 : 0}`
        });
        showToast(newState ? 'Port 500 Service Enabled' : 'Port 500 Service Disabled');
      } catch (e) { console.error(e); }
    }

    let selectedDrawerOta = null;
    function handleDrawerOtaFile(e) {
      const file = e.target.files[0];
      if (!file) return;
      selectedDrawerOta = file;
      document.getElementById('drawerOtaName').innerText = file.name;
      document.getElementById('drawerOtaSize').innerText = (file.size / 1024).toFixed(1) + ' KB';
      document.getElementById('drawerOtaInfo').style.display = 'flex';
      document.getElementById('drawerOtaBtn').style.display = 'block';
    }

    function uploadDrawerOta() {
      if (!selectedDrawerOta) return;
      const btn = document.getElementById('drawerOtaBtn');
      btn.disabled = true;
      btn.innerText = 'Uploading...';

      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/api/ota/upload', true);
      xhr.onload = () => {
        if (xhr.status === 200) {
          showToast('✅ Firmware Flashed! Rebooting...');
          setTimeout(() => window.location.reload(), 10000);
        } else {
          showToast('❌ Flash Failed: ' + xhr.responseText);
          btn.disabled = false;
          btn.innerText = 'Flash Firmware (Port 80)';
        }
      };
      const fd = new FormData();
      fd.append('firmware', selectedDrawerOta, selectedDrawerOta.name);
      xhr.send(fd);
    }

    function updateChannelVisuals(ch, state) {
      const card = document.getElementById(`relayCard${ch}`);
      const toggle = document.getElementById(`appleToggle${ch}`);
      const tile = document.getElementById(`tile${ch}`);
      const stateLbl = document.getElementById(`rStateLabel${ch}`);
      const tileState = document.getElementById(`tileState${ch}`);

      if (card) card.classList.toggle('active', state);
      if (toggle) toggle.classList.toggle('active', state);
      if (tile) tile.classList.toggle('active', state);
      if (stateLbl) stateLbl.innerText = state ? 'ACTIVE (ON)' : 'STANDBY (OFF)';
      if (tileState) tileState.innerText = state ? 'ON' : 'OFF';
    }

    function formatSeconds(sec) {
      const m = Math.floor(sec / 60);
      const s = sec % 60;
      return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
    }

    // Main Live Status Poll Loop (every 1 second)
    async function pollStatus() {
      try {
        const res = await fetch('/api/status');
        if (!res.ok) return;
        const data = await res.json();

        // 1. Clock & Sync
        if (data.time) {
          document.getElementById('navClockText').innerText = data.time.clock || '00:00:00';
          const dot = document.getElementById('navSyncDot');
          dot.classList.toggle('unsynced', !data.time.synced);
        }

        // 2. Relay Channels (1 to 4)
        let totalWattsRunning = 0;
        let lifetimeTotalKwh = 0;

        for (let ch = 1; ch <= 4; ch++) {
          const rKey = `r${ch}`;
          if (data[rKey]) {
            const r = data[rKey];
            relayData[rKey] = r;

            // Visuals
            updateChannelVisuals(ch, r.state);

            // Names & Watts
            document.getElementById(`tileName${ch}`).innerText = r.name;
            document.getElementById(`rNameText${ch}`).innerText = r.name;
            document.getElementById(`tilePower${ch}`).innerText = r.watts.toFixed(0) + 'W';
            document.getElementById(`rRatedWatts${ch}`).innerText = r.watts.toFixed(0) + 'W';

            if (r.state) {
              totalWattsRunning += r.watts;
            }
            lifetimeTotalKwh += (r.total_sec / 3600.0) * (r.watts / 1000.0);

            // Live Countdown Badge (Timer or Cycle)
            const pill = document.getElementById(`rCountdownPill${ch}`);
            if (r.timer && r.timer.active) {
              pill.style.display = 'inline-block';
              const phase = r.timer.in_delay ? 'Delay: ' : 'ON: ';
              pill.innerText = '⏱ ' + phase + formatSeconds(r.timer.remaining);
            } else if (r.cycle && r.cycle.active) {
              pill.style.display = 'inline-block';
              const cPhase = r.cycle.in_on ? 'Loop ON: ' : 'Loop OFF: ';
              pill.innerText = '🔁 ' + cPhase + formatSeconds(r.cycle.remaining);
            } else {
              pill.style.display = 'none';
            }
          }
        }

        document.getElementById('totalEnergyKwh').innerText = `${lifetimeTotalKwh.toFixed(3)} kWh Total`;

        // 3. Power Saver / Auto-Sleep Status
        if (data.power) {
          const banner = document.getElementById('sleepBanner');
          const statusTxt = document.getElementById('sleepBannerStatus');
          const descTxt = document.getElementById('sleepBannerDesc');
          const timerTxt = document.getElementById('sleepBannerTimer');
          const wakeBtn = document.getElementById('sleepWakeBtn');

          if (data.power.stay_on) {
            statusTxt.innerText = 'Power Saver: Always-On';
            descTxt.innerText = 'System stays awake permanently. No sleep interruption.';
            timerTxt.innerText = '';
            wakeBtn.style.display = 'none';
          } else if (data.power.sleeping) {
            statusTxt.innerText = 'Power Saver: Sleeping';
            descTxt.innerText = 'AP is in low-power sleep. Wakes immediately on timer or schedule.';
            timerTxt.innerText = formatSeconds(data.power.next_state_sec);
            wakeBtn.style.display = 'inline-block';
          } else {
            statusTxt.innerText = 'Power Saver: Auto-Sleep Window';
            descTxt.innerText = 'System will rest after inactivity window.';
            timerTxt.innerText = formatSeconds(data.power.next_state_sec);
            wakeBtn.style.display = 'none';
          }
        }

        // 4. Telemetry
        if (data.telem) {
          const upSec = data.telem.uptime_sec;
          const upH = Math.floor(upSec / 3600);
          const upM = Math.floor((upSec % 3600) / 60);
          document.getElementById('metricUptime').innerText = `${upH}h ${upM}m`;
          document.getElementById('metricHeap').innerText = (data.telem.free_heap / 1024).toFixed(1) + ' KB';
          document.getElementById('metricTemp').innerText = data.telem.temp_c.toFixed(1) + ' °C';
          document.getElementById('metricClients').innerText = `${data.telem.clients} connected`;
          document.getElementById('metricRelayTicks').innerText = data.telem.rtos_relay_ticks || 0;
          document.getElementById('metricPowerTicks').innerText = data.telem.rtos_power_ticks || 0;
        }

        // 5. Event Logs
        if (data.logs && Array.isArray(data.logs)) {
          const logBox = document.getElementById('eventLogBox');
          logBox.innerHTML = data.logs.map(l => `<div class="log-line"><span class="log-time">[${l.time}]</span><span class="log-msg">${l.msg}</span></div>`).join('');
        }

        // 6. Port 500 status
        if (typeof data.port500 !== 'undefined') {
          const btn = document.getElementById('port500ToggleBtn');
          const link = document.getElementById('port500Link');
          btn.innerText = data.port500 ? 'Disable' : 'Enable';
          link.style.display = data.port500 ? 'block' : 'none';
        }

      } catch (err) {
        console.warn('Status poll error:', err);
      }
    }

    // Startup Initialization
    window.addEventListener('DOMContentLoaded', () => {
      buildRelayCards();
      pollStatus();
      setInterval(pollStatus, 1000);

      // Auto time sync on first page load from mobile
      setTimeout(syncPhoneTime, 1200);
    });
  </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGES_H
