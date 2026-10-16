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
  <title>ESP32 OTA Firmware Flashing Station (Port 500)</title>
  <style>
    :root {
      --bg: #131722;
      --bg-gradient: radial-gradient(circle at 20% 20%, rgba(148, 163, 184, 0.09) 0%, transparent 50%), radial-gradient(circle at 80% 80%, rgba(71, 85, 105, 0.08) 0%, transparent 50%);
      --card-bg: rgba(28, 35, 51, 0.92);
      --card-inner-bg: #161c2b;
      --card-border: rgba(255, 255, 255, 0.15);
      --primary: #00e5ff;
      --primary-glow: rgba(0, 229, 255, 0.4);
      --success: #00e676;
      --success-glow: rgba(0, 230, 118, 0.4);
      --danger: #ff3366;
      --warning: #ffb703;
      --text: #f8fafc;
      --text-sub: #cbd5e1;
      --text-muted: #94a3b8;
      --input-bg: #0f1420;
      --card-radius: 16px;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; }
    body { background-color: var(--bg); background-image: var(--bg-gradient); color: var(--text); min-height: 100vh; padding: 20px 16px; display: flex; flex-direction: column; align-items: center; justify-content: center; }
    .container { width: 100%; max-width: 520px; display: flex; flex-direction: column; gap: 18px; }
    .card { background: var(--card-bg); backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px); border: 1px solid var(--card-border); border-radius: var(--card-radius); padding: 22px; display: flex; flex-direction: column; gap: 16px; box-shadow: 0 10px 35px rgba(0,0,0,0.4); }
    .header-box { display: flex; align-items: center; gap: 12px; }
    .logo-icon { width: 42px; height: 42px; border-radius: 12px; background: linear-gradient(135deg, var(--primary), #3b82f6); display: flex; align-items: center; justify-content: center; font-size: 22px; box-shadow: 0 0 16px var(--primary-glow); }
    .title-box h1 { font-size: 18px; font-weight: 700; color: #fff; }
    .title-box p { font-size: 12px; color: var(--text-sub); }
    .badge { display: inline-flex; align-items: center; gap: 6px; font-family: monospace; font-size: 11px; font-weight: 700; background: rgba(0, 229, 255, 0.15); border: 1px solid var(--primary); color: var(--primary); padding: 4px 10px; border-radius: 8px; width: fit-content; }
    .dropzone { border: 2px dashed rgba(0, 229, 255, 0.4); background: var(--card-inner-bg); border-radius: 14px; padding: 26px 16px; text-align: center; cursor: pointer; transition: all 0.2s; display: flex; flex-direction: column; align-items: center; gap: 8px; }
    .dropzone:hover, .dropzone.dragover { border-color: var(--primary); background: rgba(0, 229, 255, 0.06); transform: scale(1.01); }
    .drop-icon { font-size: 32px; }
    .file-info { display: none; background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 10px; padding: 12px 14px; font-size: 12px; color: #fff; justify-content: space-between; align-items: center; }
    .btn { padding: 12px 20px; border-radius: 10px; font-size: 14px; font-weight: 800; border: none; cursor: pointer; transition: all 0.2s; display: flex; align-items: center; justify-content: center; gap: 8px; }
    .btn:active { transform: scale(0.97); }
    .btn-primary { background: linear-gradient(135deg, var(--primary), #00b0ff); color: #000; box-shadow: 0 4px 16px var(--primary-glow); }
    .btn-primary:disabled { background: #334155; color: #64748b; cursor: not-allowed; box-shadow: none; }
    .btn-secondary { background: rgba(255,255,255,0.08); border: 1px solid var(--card-border); color: #fff; font-size: 12px; text-decoration: none; padding: 10px 16px; border-radius: 10px; display: flex; align-items: center; justify-content: center; gap: 6px; }
    .progress-box { display: none; flex-direction: column; gap: 6px; background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 10px; padding: 12px 14px; }
    .progress-track { width: 100%; height: 10px; background: rgba(255,255,255,0.1); border-radius: 10px; overflow: hidden; }
    .progress-fill { height: 100%; background: linear-gradient(90deg, var(--primary), var(--success)); width: 0%; transition: width 0.2s; }
    .status-msg { display: none; padding: 12px 14px; border-radius: 10px; font-size: 12px; font-weight: 600; line-height: 1.4; }
    .status-success { background: rgba(0, 230, 118, 0.15); border: 1px solid var(--success); color: var(--success); }
    .status-error { background: rgba(255, 51, 102, 0.15); border: 1px solid var(--danger); color: #ff6b8b; }
    .info-card { background: rgba(255, 255, 255, 0.04); border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 10px; padding: 12px 14px; font-size: 11px; color: var(--text-sub); display: flex; flex-direction: column; gap: 4px; line-height: 1.4; }
    .info-card b { color: #fff; }
    .footer { text-align: center; font-size: 11px; color: var(--text-muted); display: flex; flex-direction: column; gap: 4px; }
  </style>
</head>
<body>
  <div class="container">
    <div class="card">
      <div class="header-box">
        <div class="logo-icon">🚀</div>
        <div class="title-box">
          <h1>OTA Firmware Flashing Station</h1>
          <p>ESP32 Dedicated Port 500 Secure Service</p>
        </div>
      </div>

      <div class="badge">🔒 Port 500 Active &bull; Current FW: v2.0.0-OTA</div>

      <input type="file" id="firmwareFile" accept=".bin" style="display:none;" onchange="handleFileSelected(event)">

      <div class="dropzone" id="dropzone" onclick="document.getElementById('firmwareFile').click()">
        <span class="drop-icon">📦</span>
        <div style="font-weight:700; font-size:14px; color:#fff;">Click or Drag & Drop Compiled .bin File</div>
        <div style="font-size:12px; color:var(--text-sub);">Select standard ESP32 binary firmware image (.bin)</div>
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

      <button class="btn btn-primary" id="uploadBtn" onclick="uploadFirmware()" disabled>⚡ Flash Firmware to ESP32</button>

      <div class="info-card">
        <div>💡 <b>LED Status Indicator Rules:</b></div>
        <div>• <b>Blue LED Blinks 5 Times:</b> Update Success! System will auto-restart.</div>
        <div>• <b>Blue LED Blinks 3 Times:</b> Update Failed. Check compiled binary.</div>
      </div>

      <a href="http://192.168.4.1/" class="btn btn-secondary">⬅️ Return to Main Web Control (Port 80)</a>
    </div>

    <div class="footer">
      <span>ESP32 Real-Time Quad Relay Controller</span>
      <span>OTA Server running on Port 500</span>
    </div>
  </div>

  <script>
    let selectedFile = null;
    const dropzone = document.getElementById('dropzone');
    const fileInput = document.getElementById('firmwareFile');
    const uploadBtn = document.getElementById('uploadBtn');

    dropzone.addEventListener('dragover', (e) => { e.preventDefault(); dropzone.classList.add('dragover'); });
    dropzone.addEventListener('dragleave', () => dropzone.classList.remove('dragover'));
    dropzone.addEventListener('drop', (e) => {
      e.preventDefault();
      dropzone.classList.remove('dragover');
      if (e.dataTransfer.files.length > 0) {
        selectFile(e.dataTransfer.files[0]);
      }
    });

    function handleFileSelected(e) {
      if (e.target.files.length > 0) {
        selectFile(e.target.files[0]);
      }
    }

    function selectFile(file) {
      if (!file.name.endsWith('.bin')) {
        alert('Please select a compiled ESP32 .bin file.');
        return;
      }
      selectedFile = file;
      document.getElementById('fileNameText').innerText = file.name;
      document.getElementById('fileSizeText').innerText = (file.size / 1024).toFixed(1) + ' KB';
      document.getElementById('fileInfoBox').style.display = 'flex';
      uploadBtn.disabled = false;
    }

    function uploadFirmware() {
      if (!selectedFile) return;
      uploadBtn.disabled = true;
      dropzone.style.pointerEvents = 'none';

      const progressBox = document.getElementById('progressContainer');
      const progressBar = document.getElementById('progressBar');
      const progressStatus = document.getElementById('progressStatusText');
      const progressPct = document.getElementById('progressPctText');
      const statusBox = document.getElementById('statusBox');

      progressBox.style.display = 'flex';
      statusBox.style.display = 'none';
      progressBar.style.width = '0%';
      progressPct.innerText = '0%';
      progressStatus.innerText = 'Uploading firmware to OTA partition...';

      const xhr = new XMLHttpRequest();
      const formData = new FormData();
      formData.append('update', selectedFile, selectedFile.name);

      xhr.upload.addEventListener('progress', (e) => {
        if (e.lengthComputable) {
          const pct = Math.round((e.loaded / e.total) * 100);
          progressBar.style.width = pct + '%';
          progressPct.innerText = pct + '%';
          if (pct === 100) {
            progressStatus.innerText = 'Flashing Flash Partition & Validating CRC...';
          }
        }
      });

      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4) {
          if (xhr.status === 200) {
            statusBox.className = 'status-msg status-success';
            statusBox.innerHTML = '🎉 <b>OTA Update Success!</b><br>Blue LED is blinking <b>5 times</b>.<br>Device is restarting with new firmware in 3 seconds...';
            statusBox.style.display = 'block';
            progressStatus.innerText = 'Flashing Complete!';
            setTimeout(() => { window.location.href = 'http://192.168.4.1/'; }, 4500);
          } else {
            statusBox.className = 'status-msg status-error';
            statusBox.innerHTML = '❌ <b>OTA Update Failed!</b><br>Blue LED is blinking <b>3 times</b>.<br>Error: ' + (xhr.responseText || 'Verification failed. Please retry.');
            statusBox.style.display = 'block';
            uploadBtn.disabled = false;
            dropzone.style.pointerEvents = 'auto';
            progressStatus.innerText = 'Flash Error!';
          }
        }
      };

      xhr.onerror = function() {
        statusBox.className = 'status-msg status-error';
        statusBox.innerHTML = '❌ <b>Network/Upload Error!</b><br>Blue LED blinking 3 times on failure. Check connection.';
        statusBox.style.display = 'block';
        uploadBtn.disabled = false;
        dropzone.style.pointerEvents = 'auto';
      };

      xhr.open('POST', '/update', true);
      xhr.send(formData);
    }
  </script>
</body>
</html>
)rawliteral";

// =====================================================================
// MAIN WEB DASHBOARD UI (PORT 80)
// =====================================================================
const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ESP32 Real-Time Quad Relay & Telegram Control System</title>
  <style>
    :root {
      --bg: #131722;
      --bg-gradient: radial-gradient(circle at 15% 15%, rgba(148, 163, 184, 0.08) 0%, transparent 45%), radial-gradient(circle at 85% 85%, rgba(71, 85, 105, 0.08) 0%, transparent 45%);
      --card-bg: rgba(28, 35, 51, 0.90);
      --card-inner-bg: #161c2c;
      --card-border: rgba(255, 255, 255, 0.14);
      --primary: #00e5ff;
      --primary-glow: rgba(0, 229, 255, 0.45);
      --success: #00e676;
      --success-glow: rgba(0, 230, 118, 0.45);
      --danger: #ff3366;
      --danger-glow: rgba(255, 51, 102, 0.45);
      --warning: #ffb703;
      --tg-color: #29b6f6;
      --tg-glow: rgba(41, 182, 246, 0.45);
      --text: #f8fafc;
      --text-sub: #cbd5e1;
      --text-muted: #94a3b8;
      --input-bg: #0f1420;
      --card-radius: 18px;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; }
    body { background-color: var(--bg); background-image: var(--bg-gradient); color: var(--text); min-height: 100vh; padding: 16px 12px 30px; display: flex; flex-direction: column; align-items: center; overflow-x: hidden; }
    .container { width: 100%; max-width: 680px; display: flex; flex-direction: column; gap: 14px; }
    
    /* Header */
    header { background: var(--card-bg); backdrop-filter: blur(16px); -webkit-backdrop-filter: blur(16px); border: 1px solid var(--card-border); border-radius: var(--card-radius); padding: 14px 18px; display: flex; justify-content: space-between; align-items: center; box-shadow: 0 10px 30px rgba(0,0,0,0.45); position: sticky; top: 12px; z-index: 50; }
    .header-left { display: flex; align-items: center; gap: 12px; }
    .menu-btn { background: rgba(255,255,255,0.08); border: 1px solid var(--card-border); color: #fff; width: 40px; height: 40px; border-radius: 12px; cursor: pointer; display: flex; align-items: center; justify-content: center; font-size: 20px; transition: all 0.2s; }
    .menu-btn:active { transform: scale(0.92); background: rgba(0, 229, 255, 0.2); border-color: var(--primary); }
    .logo-box { display: flex; align-items: center; gap: 10px; }
    .logo-icon { width: 38px; height: 38px; border-radius: 10px; background: linear-gradient(135deg, var(--primary), #3b82f6); display: flex; align-items: center; justify-content: center; font-weight: 800; font-size: 20px; color: #000; box-shadow: 0 0 16px var(--primary-glow); }
    .title-box h1 { font-size: 16px; font-weight: 800; color: #fff; letter-spacing: -0.2px; line-height: 1.2; }
    .title-box p { font-size: 11px; color: var(--text-sub); }
    .header-actions { display: flex; gap: 8px; align-items: center; }
    .tg-pill { background: rgba(41, 182, 246, 0.15); border: 1px solid var(--tg-color); color: var(--tg-color); font-size: 11px; font-weight: 700; padding: 6px 12px; border-radius: 20px; cursor: pointer; display: flex; align-items: center; gap: 6px; transition: all 0.2s; }
    .tg-pill:active { transform: scale(0.94); background: var(--tg-color); color: #000; }
    .tg-dot { width: 8px; height: 8px; border-radius: 50%; background: #64748b; }
    .tg-dot.online { background: var(--success); box-shadow: 0 0 8px var(--success-glow); }
    .btn-icon { background: rgba(255,255,255,0.08); border: 1px solid var(--card-border); color: #fff; width: 40px; height: 40px; border-radius: 12px; cursor: pointer; display: flex; align-items: center; justify-content: center; font-size: 18px; transition: all 0.2s; }
    .btn-icon:active { transform: scale(0.92); }

    /* Section Divider Banners (Distinguish Frontend vs Backend) */
    .section-banner { display: flex; justify-content: space-between; align-items: center; padding: 10px 14px; border-radius: 12px; margin-top: 4px; box-shadow: 0 4px 16px rgba(0,0,0,0.25); }
    .section-banner-left { display: flex; align-items: center; gap: 10px; flex-wrap: wrap; }
    .section-badge { font-family: monospace; font-size: 11px; font-weight: 800; padding: 3px 8px; border-radius: 6px; letter-spacing: 0.4px; }
    .section-banner-text { font-size: 11px; color: var(--text-sub); font-weight: 600; }
    .section-pill { font-family: monospace; font-size: 10px; font-weight: 700; padding: 2px 8px; border-radius: 10px; }
    
    .frontend-banner { background: linear-gradient(90deg, rgba(0, 229, 255, 0.12), rgba(0, 230, 118, 0.08)); border: 1px solid rgba(0, 229, 255, 0.35); }
    .frontend-badge { background: rgba(0, 229, 255, 0.22); border: 1px solid var(--primary); color: var(--primary); }
    .frontend-pill { background: rgba(0, 230, 118, 0.15); border: 1px solid var(--success); color: var(--success); }

    .backend-banner { background: linear-gradient(90deg, rgba(168, 85, 247, 0.15), rgba(59, 130, 246, 0.10)); border: 1px solid rgba(168, 85, 247, 0.4); margin-top: 10px; }
    .backend-badge { background: rgba(168, 85, 247, 0.25); border: 1px solid #c084fc; color: #e9d5ff; }
    .backend-pill { background: rgba(168, 85, 247, 0.15); border: 1px solid #a855f7; color: #d8b4fe; }

    .nav-section-title { font-size: 10px; font-weight: 800; color: var(--text-muted); text-transform: uppercase; letter-spacing: 0.8px; padding: 8px 12px 2px; }

    /* Sidenav Drawer */
    .sidenav-overlay { position: fixed; top: 0; left: 0; width: 100vw; height: 100vh; background: rgba(0,0,0,0.7); backdrop-filter: blur(8px); -webkit-backdrop-filter: blur(8px); z-index: 200; opacity: 0; pointer-events: none; transition: opacity 0.3s ease; }
    .sidenav-overlay.open { opacity: 1; pointer-events: auto; }
    .sidenav { position: fixed; top: 0; left: -320px; width: 300px; height: 100vh; background: #151b29; border-right: 1px solid var(--card-border); z-index: 201; padding: 22px 18px; display: flex; flex-direction: column; justify-content: space-between; transition: left 0.3s cubic-bezier(0.4, 0, 0.2, 1); box-shadow: 10px 0 35px rgba(0,0,0,0.6); overflow-y: auto; }
    .sidenav.open { left: 0; }
    .sidenav-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--card-border); padding-bottom: 16px; margin-bottom: 16px; }
    .sidenav-brand { display: flex; align-items: center; gap: 10px; }
    .sidenav-nav { display: flex; flex-direction: column; gap: 6px; }
    .nav-link { display: flex; align-items: center; gap: 12px; padding: 10px 14px; border-radius: 12px; color: var(--text-sub); text-decoration: none; font-size: 13px; font-weight: 700; transition: all 0.2s; background: transparent; border: 1px solid transparent; }
    .nav-link:hover, .nav-link:active { background: rgba(0, 229, 255, 0.1); color: var(--primary); border-color: rgba(0, 229, 255, 0.3); }
    .sidenav-footer { background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 12px; padding: 12px; display: flex; flex-direction: column; gap: 6px; font-size: 11px; color: var(--text-muted); }

    /* Time & Status Bar */
    .status-bar { background: var(--card-bg); backdrop-filter: blur(12px); border: 1px solid var(--card-border); border-radius: 14px; padding: 10px 16px; display: flex; justify-content: space-between; align-items: center; font-size: 13px; box-shadow: 0 4px 16px rgba(0,0,0,0.25); }
    .time-display { display: flex; align-items: center; gap: 8px; font-weight: 800; color: var(--primary); font-size: 14px; }
    .sync-btn { background: rgba(0, 229, 255, 0.15); border: 1px solid var(--primary); color: var(--primary); padding: 5px 12px; border-radius: 8px; font-size: 11px; font-weight: 700; cursor: pointer; transition: all 0.2s; }
    .sync-btn:active { background: var(--primary); color: #000; }

    /* Quick Relay Shortcuts Bar (Screen Shortcut) */
    .shortcuts-card { background: linear-gradient(135deg, rgba(0, 229, 255, 0.08), rgba(0, 230, 118, 0.06)); border: 1px solid rgba(0, 229, 255, 0.3); border-radius: var(--card-radius); padding: 14px 16px; display: flex; flex-direction: column; gap: 10px; box-shadow: 0 6px 24px rgba(0,0,0,0.3); }
    .shortcuts-header { display: flex; justify-content: space-between; align-items: center; }
    .shortcuts-title { font-size: 13px; font-weight: 800; color: #fff; display: flex; align-items: center; gap: 6px; }
    .master-btn-group { display: flex; gap: 6px; }
    .master-pill { font-size: 10px; font-weight: 800; padding: 4px 10px; border-radius: 6px; border: none; cursor: pointer; transition: all 0.2s; text-transform: uppercase; }
    .master-on { background: var(--success); color: #000; }
    .master-off { background: rgba(255, 51, 102, 0.2); border: 1px solid var(--danger); color: #ff6b8b; }
    .master-off:active { background: var(--danger); color: #fff; }
    .shortcuts-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 8px; }
    .relay-shortcut-btn { background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 12px; padding: 10px 6px; display: flex; flex-direction: column; align-items: center; gap: 4px; cursor: pointer; transition: all 0.2s; user-select: none; }
    .relay-shortcut-btn:active { transform: scale(0.94); }
    .relay-shortcut-btn.on { background: linear-gradient(135deg, rgba(0, 230, 118, 0.25), rgba(0, 229, 255, 0.15)); border-color: var(--success); box-shadow: 0 0 16px var(--success-glow); }
    .sc-icon { font-size: 18px; }
    .sc-name { font-size: 11px; font-weight: 700; color: #fff; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; max-width: 100%; }
    .sc-badge { font-size: 9px; font-weight: 800; padding: 2px 6px; border-radius: 4px; font-family: monospace; }
    .sc-badge.off { background: rgba(255,255,255,0.1); color: var(--text-muted); }
    .sc-badge.on { background: var(--success); color: #000; }

    /* Auto-Sleep Live Countdown Card */
    .sleep-countdown-card { background: linear-gradient(135deg, rgba(255, 183, 3, 0.10), rgba(0, 229, 255, 0.08)); border: 1px solid rgba(255, 183, 3, 0.35); border-radius: 14px; padding: 12px 16px; display: flex; justify-content: space-between; align-items: center; box-shadow: 0 4px 18px rgba(0,0,0,0.25); }
    .sleep-left { display: flex; flex-direction: column; gap: 3px; max-width: 75%; }
    .sleep-title { font-size: 13px; font-weight: 700; color: #fff; display: flex; align-items: center; gap: 6px; }
    .sleep-countdown-val { font-family: monospace; font-size: 15px; font-weight: 800; color: var(--warning); }
    .sleep-sub { font-size: 11px; color: var(--text-sub); }

    /* Total Energy Card */
    .energy-total-card { background: linear-gradient(135deg, rgba(0, 230, 118, 0.10), rgba(0, 229, 255, 0.08)); border: 1px solid rgba(0, 230, 118, 0.35); border-radius: 14px; padding: 12px 16px; display: flex; flex-direction: column; gap: 8px; box-shadow: 0 4px 20px rgba(0,0,0,0.25); }
    .energy-total-header { display: flex; justify-content: space-between; align-items: center; }
    .energy-title { font-size: 13px; font-weight: 800; color: #fff; display: flex; align-items: center; gap: 6px; }
    .energy-val-badge { font-family: monospace; font-size: 15px; font-weight: 800; color: var(--success); background: rgba(0, 230, 118, 0.18); border: 1px solid var(--success); padding: 2px 10px; border-radius: 8px; box-shadow: 0 0 12px var(--success-glow); }
    .energy-subgrid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .energy-subitem { background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 8px; padding: 6px 10px; display: flex; flex-direction: column; gap: 2px; }
    .energy-subtitle { font-size: 10px; color: var(--text-sub); font-weight: 600; }
    .energy-subval { font-size: 12px; font-weight: 700; color: #fff; }

    /* Accordion Dynamic Relay Cards */
    .relay-card { background: var(--card-bg); backdrop-filter: blur(14px); -webkit-backdrop-filter: blur(14px); border: 1px solid var(--card-border); border-radius: var(--card-radius); display: flex; flex-direction: column; overflow: hidden; box-shadow: 0 6px 22px rgba(0,0,0,0.3); transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); }
    .relay-card.active { border-color: rgba(0, 230, 118, 0.6); box-shadow: 0 0 24px rgba(0, 230, 118, 0.22); }
    
    /* Relay Card Header (Always visible) */
    .relay-head-bar { padding: 14px 16px; display: flex; justify-content: space-between; align-items: center; cursor: pointer; user-select: none; background: transparent; transition: background 0.2s; }
    .relay-head-bar:hover { background: rgba(255,255,255,0.03); }
    .relay-info-left { display: flex; align-items: center; gap: 10px; }
    .relay-ch-badge { width: 34px; height: 34px; border-radius: 10px; background: rgba(255,255,255,0.08); border: 1px solid var(--card-border); display: flex; align-items: center; justify-content: center; font-weight: 800; font-size: 12px; color: var(--primary); }
    .relay-title-group { display: flex; flex-direction: column; gap: 2px; }
    .relay-title { font-size: 15px; font-weight: 800; color: #fff; }
    .relay-active-pill { font-size: 10px; font-weight: 700; color: var(--warning); display: none; align-items: center; gap: 4px; }
    .relay-head-right { display: flex; align-items: center; gap: 10px; }
    .expand-btn { background: rgba(255,255,255,0.08); border: 1px solid var(--card-border); color: var(--text-sub); font-size: 11px; font-weight: 700; padding: 6px 10px; border-radius: 8px; display: flex; align-items: center; gap: 4px; transition: all 0.2s; }
    .expand-caret { transition: transform 0.3s; }
    .relay-card.open .expand-caret { transform: rotate(180deg); color: var(--primary); }
    .relay-card.open .expand-btn { background: rgba(0, 229, 255, 0.15); border-color: var(--primary); color: #fff; }

    /* Collapsible Body (Accordion) */
    .relay-body { max-height: 0; overflow: hidden; transition: max-height 0.4s cubic-bezier(0.4, 0, 0.2, 1), padding 0.3s ease; padding: 0 16px; border-top: 1px solid transparent; display: flex; flex-direction: column; gap: 12px; }
    .relay-card.open .relay-body { max-height: 700px; padding: 14px 16px 18px; border-top-color: var(--card-border); }

    /* Main Toggle Switch */
    .toggle-switch { position: relative; width: 52px; height: 28px; display: inline-block; }
    .toggle-switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #334155; transition: .3s; border-radius: 30px; }
    .slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .3s; border-radius: 50%; }
    input:checked + .slider { background-color: var(--success); box-shadow: 0 0 14px var(--success-glow); }
    input:checked + .slider:before { transform: translateX(24px); }

    /* Tabs inside Relay */
    .tab-nav { display: flex; gap: 6px; background: var(--card-inner-bg); border: 1px solid var(--card-border); padding: 4px; border-radius: 10px; }
    .tab-btn { flex: 1; padding: 7px 0; background: transparent; border: none; color: var(--text-sub); font-size: 11px; font-weight: 700; border-radius: 7px; cursor: pointer; transition: all 0.2s; }
    .tab-btn.active { background: rgba(255,255,255,0.18); color: #ffffff; }
    .tab-pane { display: none; flex-direction: column; gap: 10px; padding-top: 4px; }
    .tab-pane.active { display: flex; }

    /* Inputs & Forms */
    .form-group { display: flex; flex-direction: column; gap: 4px; }
    .form-label { font-size: 12px; color: var(--text-sub); font-weight: 700; }
    .form-hint { font-size: 10px; color: var(--text-muted); line-height: 1.3; }
    .input-row { display: flex; gap: 8px; }
    .input-box { flex: 1; background: var(--input-bg); border: 1px solid #2d3748; color: #fff; padding: 9px 12px; border-radius: 8px; font-size: 13px; outline: none; transition: border-color 0.2s; }
    .input-box:focus { border-color: var(--primary); box-shadow: 0 0 8px var(--primary-glow); }

    /* Weekday Selection Bar */
    .weekday-bar { display: flex; gap: 4px; justify-content: space-between; margin-top: 4px; }
    .day-btn { flex: 1; padding: 6px 0; font-size: 10px; font-weight: 700; background: var(--input-bg); border: 1px solid #2d3748; color: var(--text-sub); border-radius: 6px; cursor: pointer; text-align: center; transition: all 0.2s; user-select: none; }
    .day-btn.selected { background: linear-gradient(135deg, var(--primary), #00b0ff); color: #000; border-color: var(--primary); box-shadow: 0 0 8px var(--primary-glow); }
    .day-presets { display: flex; gap: 6px; }
    .preset-btn { font-size: 10px; font-weight: 700; background: rgba(255,255,255,0.06); border: 1px solid var(--card-border); color: var(--text-sub); padding: 2px 7px; border-radius: 4px; cursor: pointer; }

    /* Buttons */
    .btn { padding: 9px 14px; border-radius: 8px; font-size: 12px; font-weight: 800; border: none; cursor: pointer; transition: all 0.2s; display: flex; align-items: center; justify-content: center; gap: 6px; }
    .btn:active { transform: scale(0.96); }
    .btn-primary { background: linear-gradient(135deg, var(--primary), #00b0ff); color: #000; box-shadow: 0 4px 14px var(--primary-glow); }
    .btn-danger { background: rgba(255, 51, 102, 0.2); border: 1px solid var(--danger); color: #ff6b8b; }
    .btn-danger:active { background: var(--danger); color: #fff; }
    .btn-success { background: var(--success); color: #000; }
    .btn-tg { background: linear-gradient(135deg, var(--tg-color), #0288d1); color: #000; box-shadow: 0 4px 14px var(--tg-glow); }

    /* Live Progress Box */
    .live-prog-box { background: rgba(0, 229, 255, 0.1); border: 1px solid rgba(0, 229, 255, 0.3); border-radius: 10px; padding: 10px 12px; display: flex; flex-direction: column; gap: 6px; }
    .live-prog-header { display: flex; justify-content: space-between; font-size: 12px; font-weight: 700; }
    .live-prog-val { font-family: monospace; font-size: 15px; font-weight: 800; color: var(--primary); }
    .progress-track { width: 100%; height: 6px; background: rgba(255,255,255,0.15); border-radius: 10px; overflow: hidden; }
    .progress-fill { height: 100%; background: linear-gradient(90deg, var(--primary), var(--success)); width: 0%; transition: width 0.4s ease; }

    /* Stats Grid */
    .stats-row { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; font-size: 12px; }
    .stat-item { background: var(--card-inner-bg); border: 1px solid var(--card-border); padding: 8px 10px; border-radius: 8px; display: flex; flex-direction: column; gap: 2px; }
    .stat-val { font-weight: 800; font-size: 13px; color: var(--primary); font-family: monospace; }

    /* Telemetry Card */
    .telemetry-card { background: var(--card-bg); backdrop-filter: blur(12px); border: 1px solid var(--card-border); border-radius: var(--card-radius); padding: 16px; display: flex; flex-direction: column; gap: 12px; box-shadow: 0 8px 24px rgba(0,0,0,0.3); }
    .telemetry-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 8px; }
    .telem-box { background: var(--card-inner-bg); border: 1px solid var(--card-border); border-radius: 10px; padding: 8px 10px; display: flex; flex-direction: column; gap: 2px; }
    .telem-title { font-size: 10px; color: var(--text-sub); font-weight: 600; }
    .telem-val { font-size: 13px; font-weight: 800; color: #fff; font-family: monospace; }

    /* Event Logs */
    .event-log-box { background: #0d121c; border: 1px solid var(--card-border); border-radius: 10px; padding: 10px 12px; font-family: monospace; font-size: 11px; max-height: 120px; overflow-y: auto; display: flex; flex-direction: column; gap: 4px; }
    .event-entry { display: flex; gap: 8px; color: #cbd5e1; border-bottom: 1px solid rgba(255,255,255,0.04); padding: 2px 0; }
    .event-time { color: var(--primary); font-weight: 700; min-width: 70px; }

    .master-emergency-btn { width: 100%; background: linear-gradient(135deg, #e11d48, #ff1744); color: #fff; font-size: 13px; font-weight: 800; padding: 12px; border-radius: 12px; border: none; cursor: pointer; box-shadow: 0 4px 20px var(--danger-glow); transition: transform 0.2s; }
    .master-emergency-btn:active { transform: scale(0.97); }

    /* Modals */
    .modal-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(10, 13, 20, 0.82); backdrop-filter: blur(10px); display: none; align-items: center; justify-content: center; z-index: 100; padding: 16px; }
    .modal-overlay.open { display: flex; }
    .modal { background: #171e2e; border: 1px solid var(--card-border); border-radius: var(--card-radius); width: 100%; max-width: 540px; max-height: 90vh; overflow-y: auto; padding: 20px; display: flex; flex-direction: column; gap: 14px; box-shadow: 0 16px 45px rgba(0,0,0,0.7); }
    .modal-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--card-border); padding-bottom: 12px; }
    .modal-title { font-size: 16px; font-weight: 800; color: #fff; display: flex; align-items: center; gap: 8px; }
    .close-btn { background: transparent; border: none; color: var(--text-sub); font-size: 22px; cursor: pointer; }

    /* Footer */
    .footer-card { background: var(--card-bg); backdrop-filter: blur(12px); border: 1px solid var(--card-border); border-radius: var(--card-radius); padding: 14px 18px; display: flex; flex-direction: column; gap: 8px; font-size: 11px; color: var(--text-muted); }
    .footer-top { display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 8px; }
    .version-badge { font-family: monospace; font-size: 10px; font-weight: 700; background: rgba(0, 229, 255, 0.15); border: 1px solid var(--primary); color: var(--primary); padding: 3px 8px; border-radius: 6px; }

    /* Toast */
    #toast { position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%) translateY(100px); background: #1e293b; border: 1px solid var(--primary); color: #fff; padding: 10px 20px; border-radius: 30px; font-size: 12px; font-weight: 700; box-shadow: 0 10px 30px rgba(0,0,0,0.6); opacity: 0; transition: all 0.3s ease; z-index: 300; pointer-events: none; }
    #toast.show { transform: translateX(-50%) translateY(0); opacity: 1; }
  </style>
</head>
<body>

  <!-- Sidenav Drawer Overlay -->
  <div class="sidenav-overlay" id="sidenavOverlay" onclick="toggleSidenav(false)"></div>
  <aside class="sidenav" id="sidenavDrawer">
    <div>
      <div class="sidenav-header">
        <div class="sidenav-brand">
          <div class="logo-icon">⚡</div>
          <div>
            <div style="font-weight:800; color:#fff; font-size:14px;">ESP32 RTOS</div>
            <div style="font-size:11px; color:var(--text-sub);">Quad Relay & Telegram</div>
          </div>
        </div>
        <button class="close-btn" onclick="toggleSidenav(false)">&times;</button>
      </div>

      <nav class="sidenav-nav">
        <div class="nav-section-title">🎛️ FRONTEND CONTROLS</div>
        <a href="#frontendSection" class="nav-link" onclick="toggleSidenav(false)">⚡ Relay Control & Timers</a>
        <a href="#shortcutsSection" class="nav-link" onclick="toggleSidenav(false)">🚀 Quick 1-Tap Shortcuts</a>
        <a href="#energySection" class="nav-link" onclick="toggleSidenav(false)">📊 Energy & Power Consumption</a>

        <div class="nav-section-title">⚙️ BACKEND & SERVICES</div>
        <a href="#powerSection" class="nav-link" onclick="toggleSidenav(false)">🔋 Power & Auto-Sleep Manager</a>
        <a href="#telemetrySection" class="nav-link" onclick="toggleSidenav(false)">📊 FreeRTOS System Telemetry</a>
        <a href="javascript:void(0)" class="nav-link" onclick="openTelegramModal(); toggleSidenav(false);">✈️ Telegram Bot Hub</a>
        <a href="javascript:void(0)" class="nav-link" onclick="openSettingsModal(); toggleSidenav(false);">⚙️ System Settings & OTA</a>
        <a href="http://192.168.4.1:500" target="_blank" class="nav-link">🚀 Port 500 Dedicated Flasher</a>
      </nav>
    </div>

    <div class="sidenav-footer">
      <div>🌡️ Chip Temp: <b id="sideTemp" style="color:var(--primary);">-- °C</b></div>
      <div>💾 Free RAM: <b id="sideHeap" style="color:#fff;">-- KB</b></div>
      <div>⏱️ Uptime: <b id="sideUptime" style="color:var(--text-sub);">--</b></div>
      <div style="margin-top:4px; font-size:10px; color:var(--text-muted);">FW: v2.0.0-OTA &bull; Dual-Core RTOS</div>
    </div>
  </aside>

  <div class="container">
    <!-- Header -->
    <header>
      <div class="header-left">
        <button class="menu-btn" onclick="toggleSidenav(true)" title="Navigation Menu">☰</button>
        <div class="logo-box">
          <div class="logo-icon">⚡</div>
          <div class="title-box">
            <h1>ESP32 RTOS Quad Relay</h1>
            <p>FreeRTOS & Telegram IoT Controller</p>
          </div>
        </div>
      </div>
      <div class="header-actions">
        <div class="tg-pill" onclick="openTelegramModal()" title="Telegram Bot Settings">
          <div class="tg-dot" id="tgHeaderDot"></div>
          <span>✈️ Telegram</span>
        </div>
        <button class="btn-icon" onclick="openSettingsModal()" title="System Settings">⚙️</button>
      </div>
    </header>

    <!-- Time & Status Bar -->
    <div class="status-bar">
      <div class="time-display">
        <span>🕒</span>
        <span id="liveClock">--:--:-- --</span>
        <span id="liveDate" style="color:var(--text-sub); font-size:11px; font-weight:normal;">(Not Synced)</span>
      </div>
      <button class="sync-btn" onclick="syncPhoneTime()">📱 Sync Time</button>
    </div>

    <!-- FRONTEND CONTROLS & DASHBOARD BANNER -->
    <div class="section-banner frontend-banner" id="frontendSection">
      <div class="section-banner-left">
        <span class="section-badge frontend-badge">🎛️ FRONTEND DASHBOARD</span>
        <span class="section-banner-text">Relay Switching, Timers, Cycles & Daily Schedules</span>
      </div>
      <span class="section-pill frontend-pill">4 Channels</span>
    </div>

    <!-- Quick Relay Shortcuts Bar (Screen Shortcut) -->
    <div class="shortcuts-card" id="shortcutsSection">
      <div class="shortcuts-header">
        <span class="shortcuts-title">⚡ Quick Relay Shortcuts (1-Tap Toggle)</span>
        <div class="master-btn-group">
          <button class="master-pill master-on" onclick="masterAllOn()">All ON</button>
          <button class="master-pill master-off" onclick="killAllRelays()">All OFF</button>
        </div>
      </div>
      <div class="shortcuts-grid">
        <div class="relay-shortcut-btn" id="scR1" onclick="quickToggleRelay(1)">
          <span class="sc-icon">⚡</span>
          <span class="sc-name" id="scR1Name">Relay 1</span>
          <span class="sc-badge off" id="scR1Badge">OFF</span>
        </div>
        <div class="relay-shortcut-btn" id="scR2" onclick="quickToggleRelay(2)">
          <span class="sc-icon">⚡</span>
          <span class="sc-name" id="scR2Name">Relay 2</span>
          <span class="sc-badge off" id="scR2Badge">OFF</span>
        </div>
        <div class="relay-shortcut-btn" id="scR3" onclick="quickToggleRelay(3)">
          <span class="sc-icon">⚡</span>
          <span class="sc-name" id="scR3Name">Relay 3</span>
          <span class="sc-badge off" id="scR3Badge">OFF</span>
        </div>
        <div class="relay-shortcut-btn" id="scR4" onclick="quickToggleRelay(4)">
          <span class="sc-icon">⚡</span>
          <span class="sc-name" id="scR4Name">Relay 4</span>
          <span class="sc-badge off" id="scR4Badge">OFF</span>
        </div>
      </div>
    </div>

    <div id="relaysSection" style="display:flex; flex-direction:column; gap:12px;">
      <!-- Relay 1 Dynamic Card -->
      <div class="relay-card" id="relayCard1">
        <div class="relay-head-bar" onclick="toggleAccordion(1)">
          <div class="relay-info-left">
            <div class="relay-ch-badge">R1</div>
            <div class="relay-title-group">
              <span class="relay-title" id="r1Name">Relay 1 (D5)</span>
              <span class="relay-active-pill" id="r1ActivePill">⏱️ Active</span>
            </div>
          </div>
          <div class="relay-head-right" onclick="event.stopPropagation()">
            <label class="toggle-switch">
              <input type="checkbox" id="r1Switch" onchange="toggleRelay(1, this.checked)">
              <span class="slider"></span>
            </label>
            <button class="expand-btn" onclick="toggleAccordion(1); event.stopPropagation();">
              <span>Settings</span>
              <span class="expand-caret">▼</span>
            </button>
          </div>
        </div>

        <div class="relay-body" id="r1Body">
          <!-- Live Timer Progress -->
          <div class="live-prog-box" id="r1TimerLiveBox" style="display:none;">
            <div class="live-prog-header">
              <span id="r1TimerPhase">Timer: Active</span>
              <span class="live-prog-val" id="r1TimerCountdown">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r1TimerProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelTimer(1)">Cancel Countdown</button>
          </div>

          <!-- Live Cycle Progress -->
          <div class="live-prog-box" id="r1CycleLiveBox" style="display:none; background:rgba(0, 230, 118, 0.1); border-color:rgba(0,230,118,0.3);">
            <div class="live-prog-header">
              <span id="r1CyclePhase">🔁 Cycle Running</span>
              <span class="live-prog-val" id="r1CycleCountdown" style="color:var(--success);">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r1CycleProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelCycle(1)">Stop Cycle Loop</button>
          </div>

          <!-- Tab Navigation -->
          <div class="tab-nav">
            <button class="tab-btn active" onclick="switchTab(1, 'timer')">⏱️ Timer</button>
            <button class="tab-btn" onclick="switchTab(1, 'cycle')">🔁 Cycle</button>
            <button class="tab-btn" onclick="switchTab(1, 'sched')">📅 Schedule</button>
            <button class="tab-btn" onclick="switchTab(1, 'stats')">📊 Energy</button>
          </div>

          <!-- Tab 1: Timer (2 Inputs clearly explained) -->
          <div class="tab-pane active" id="r1TabTimer">
            <div class="form-group">
              <label class="form-label">1. Start Delay (Wait before turning ON):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r1DelayMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r1DelaySec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">Time to wait before turning ON. (Set 0 to turn ON immediately)</span>
            </div>
            <div class="form-group">
              <label class="form-label">2. ON Duration (Stay ON before turning OFF):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r1DurMin" placeholder="Mins" min="0" value="10">
                <input type="number" class="input-box" id="r1DurSec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">How long the relay stays ON before automatically turning OFF.</span>
            </div>
            <button class="btn btn-primary" onclick="setTimer(1)">Start Countdown Timer</button>
          </div>

          <!-- Tab 2: Cycle Automation -->
          <div class="tab-pane" id="r1TabCycle">
            <div class="form-group">
              <label class="form-label">ON Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r1CycOnMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r1CycOnSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">OFF Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r1CycOffMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r1CycOffSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">Total Cycles (0 = Infinite Loop):</label>
              <input type="number" class="input-box" id="r1CycTotal" placeholder="0 = Infinite Loop" min="0" value="0">
            </div>
            <button class="btn btn-primary" onclick="setCycle(1)">Start Cycle Automation</button>
          </div>

          <!-- Tab 3: Daily Schedule -->
          <div class="tab-pane" id="r1TabSched">
            <div class="form-group">
              <label class="form-label">Daily Turn ON Time (HH:MM):</label>
              <input type="time" class="input-box" id="r1SchedStart" value="08:00">
            </div>
            <div class="form-group">
              <label class="form-label">Daily Turn OFF Time (HH:MM):</label>
              <input type="time" class="input-box" id="r1SchedEnd" value="18:00">
            </div>
            <div class="form-group">
              <div style="display:flex; justify-content:space-between; align-items:center;">
                <label class="form-label">Active Weekdays:</label>
                <div class="day-presets">
                  <button type="button" class="preset-btn" onclick="selectDayPreset(1, 'all')">All</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(1, 'weekdays')">Mon-Fri</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(1, 'weekends')">Sat-Sun</button>
                </div>
              </div>
              <div class="weekday-bar" id="r1WeekdayBar">
                <div class="day-btn selected" data-day="0" onclick="toggleDay(1, 0)">Sun</div>
                <div class="day-btn selected" data-day="1" onclick="toggleDay(1, 1)">Mon</div>
                <div class="day-btn selected" data-day="2" onclick="toggleDay(1, 2)">Tue</div>
                <div class="day-btn selected" data-day="3" onclick="toggleDay(1, 3)">Wed</div>
                <div class="day-btn selected" data-day="4" onclick="toggleDay(1, 4)">Thu</div>
                <div class="day-btn selected" data-day="5" onclick="toggleDay(1, 5)">Fri</div>
                <div class="day-btn selected" data-day="6" onclick="toggleDay(1, 6)">Sat</div>
              </div>
            </div>
            <div class="input-row" style="align-items:center; justify-content:space-between; margin-top:4px;">
              <span style="font-size:12px; color:var(--text-sub); font-weight:700;">Enable Daily Schedule</span>
              <label class="toggle-switch">
                <input type="checkbox" id="r1SchedEn" onchange="setSchedule(1)">
                <span class="slider"></span>
              </label>
            </div>
          </div>

          <!-- Tab 4: Energy & Stats -->
          <div class="tab-pane" id="r1TabStats">
            <div class="stats-row">
              <div class="stat-item">
                <span class="telem-title">Total ON Runtime</span>
                <span class="stat-val" id="r1TotTime">0h 0m</span>
              </div>
              <div class="stat-item">
                <span class="telem-title">Est. Energy Consumed</span>
                <span class="stat-val" id="r1Energy">0.000 kWh</span>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- Relay 2 Dynamic Card -->
      <div class="relay-card" id="relayCard2">
        <div class="relay-head-bar" onclick="toggleAccordion(2)">
          <div class="relay-info-left">
            <div class="relay-ch-badge">R2</div>
            <div class="relay-title-group">
              <span class="relay-title" id="r2Name">Relay 2 (D18)</span>
              <span class="relay-active-pill" id="r2ActivePill">⏱️ Active</span>
            </div>
          </div>
          <div class="relay-head-right" onclick="event.stopPropagation()">
            <label class="toggle-switch">
              <input type="checkbox" id="r2Switch" onchange="toggleRelay(2, this.checked)">
              <span class="slider"></span>
            </label>
            <button class="expand-btn" onclick="toggleAccordion(2); event.stopPropagation();">
              <span>Settings</span>
              <span class="expand-caret">▼</span>
            </button>
          </div>
        </div>

        <div class="relay-body" id="r2Body">
          <!-- Live Timer Progress -->
          <div class="live-prog-box" id="r2TimerLiveBox" style="display:none;">
            <div class="live-prog-header">
              <span id="r2TimerPhase">Timer: Active</span>
              <span class="live-prog-val" id="r2TimerCountdown">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r2TimerProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelTimer(2)">Cancel Countdown</button>
          </div>

          <!-- Live Cycle Progress -->
          <div class="live-prog-box" id="r2CycleLiveBox" style="display:none; background:rgba(0, 230, 118, 0.1); border-color:rgba(0,230,118,0.3);">
            <div class="live-prog-header">
              <span id="r2CyclePhase">🔁 Cycle Running</span>
              <span class="live-prog-val" id="r2CycleCountdown" style="color:var(--success);">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r2CycleProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelCycle(2)">Stop Cycle Loop</button>
          </div>

          <!-- Tab Navigation -->
          <div class="tab-nav">
            <button class="tab-btn active" onclick="switchTab(2, 'timer')">⏱️ Timer</button>
            <button class="tab-btn" onclick="switchTab(2, 'cycle')">🔁 Cycle</button>
            <button class="tab-btn" onclick="switchTab(2, 'sched')">📅 Schedule</button>
            <button class="tab-btn" onclick="switchTab(2, 'stats')">📊 Energy</button>
          </div>

          <!-- Tab 1: Timer (2 Inputs clearly explained) -->
          <div class="tab-pane active" id="r2TabTimer">
            <div class="form-group">
              <label class="form-label">1. Start Delay (Wait before turning ON):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r2DelayMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r2DelaySec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">Time to wait before turning ON. (Set 0 to turn ON immediately)</span>
            </div>
            <div class="form-group">
              <label class="form-label">2. ON Duration (Stay ON before turning OFF):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r2DurMin" placeholder="Mins" min="0" value="15">
                <input type="number" class="input-box" id="r2DurSec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">How long the relay stays ON before automatically turning OFF.</span>
            </div>
            <button class="btn btn-primary" onclick="setTimer(2)">Start Countdown Timer</button>
          </div>

          <!-- Tab 2: Cycle Automation -->
          <div class="tab-pane" id="r2TabCycle">
            <div class="form-group">
              <label class="form-label">ON Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r2CycOnMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r2CycOnSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">OFF Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r2CycOffMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r2CycOffSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">Total Cycles (0 = Infinite Loop):</label>
              <input type="number" class="input-box" id="r2CycTotal" placeholder="0 = Infinite Loop" min="0" value="0">
            </div>
            <button class="btn btn-primary" onclick="setCycle(2)">Start Cycle Automation</button>
          </div>

          <!-- Tab 3: Daily Schedule -->
          <div class="tab-pane" id="r2TabSched">
            <div class="form-group">
              <label class="form-label">Daily Turn ON Time (HH:MM):</label>
              <input type="time" class="input-box" id="r2SchedStart" value="18:00">
            </div>
            <div class="form-group">
              <label class="form-label">Daily Turn OFF Time (HH:MM):</label>
              <input type="time" class="input-box" id="r2SchedEnd" value="22:00">
            </div>
            <div class="form-group">
              <div style="display:flex; justify-content:space-between; align-items:center;">
                <label class="form-label">Active Weekdays:</label>
                <div class="day-presets">
                  <button type="button" class="preset-btn" onclick="selectDayPreset(2, 'all')">All</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(2, 'weekdays')">Mon-Fri</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(2, 'weekends')">Sat-Sun</button>
                </div>
              </div>
              <div class="weekday-bar" id="r2WeekdayBar">
                <div class="day-btn selected" data-day="0" onclick="toggleDay(2, 0)">Sun</div>
                <div class="day-btn selected" data-day="1" onclick="toggleDay(2, 1)">Mon</div>
                <div class="day-btn selected" data-day="2" onclick="toggleDay(2, 2)">Tue</div>
                <div class="day-btn selected" data-day="3" onclick="toggleDay(2, 3)">Wed</div>
                <div class="day-btn selected" data-day="4" onclick="toggleDay(2, 4)">Thu</div>
                <div class="day-btn selected" data-day="5" onclick="toggleDay(2, 5)">Fri</div>
                <div class="day-btn selected" data-day="6" onclick="toggleDay(2, 6)">Sat</div>
              </div>
            </div>
            <div class="input-row" style="align-items:center; justify-content:space-between; margin-top:4px;">
              <span style="font-size:12px; color:var(--text-sub); font-weight:700;">Enable Daily Schedule</span>
              <label class="toggle-switch">
                <input type="checkbox" id="r2SchedEn" onchange="setSchedule(2)">
                <span class="slider"></span>
              </label>
            </div>
          </div>

          <!-- Tab 4: Energy & Stats -->
          <div class="tab-pane" id="r2TabStats">
            <div class="stats-row">
              <div class="stat-item">
                <span class="telem-title">Total ON Runtime</span>
                <span class="stat-val" id="r2TotTime">0h 0m</span>
              </div>
              <div class="stat-item">
                <span class="telem-title">Est. Energy Consumed</span>
                <span class="stat-val" id="r2Energy">0.000 kWh</span>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- Relay 3 Dynamic Card -->
      <div class="relay-card" id="relayCard3">
        <div class="relay-head-bar" onclick="toggleAccordion(3)">
          <div class="relay-info-left">
            <div class="relay-ch-badge">R3</div>
            <div class="relay-title-group">
              <span class="relay-title" id="r3Name">Relay 3 (D19)</span>
              <span class="relay-active-pill" id="r3ActivePill">⏱️ Active</span>
            </div>
          </div>
          <div class="relay-head-right" onclick="event.stopPropagation()">
            <label class="toggle-switch">
              <input type="checkbox" id="r3Switch" onchange="toggleRelay(3, this.checked)">
              <span class="slider"></span>
            </label>
            <button class="expand-btn" onclick="toggleAccordion(3); event.stopPropagation();">
              <span>Settings</span>
              <span class="expand-caret">▼</span>
            </button>
          </div>
        </div>

        <div class="relay-body" id="r3Body">
          <!-- Live Timer Progress -->
          <div class="live-prog-box" id="r3TimerLiveBox" style="display:none;">
            <div class="live-prog-header">
              <span id="r3TimerPhase">Timer: Active</span>
              <span class="live-prog-val" id="r3TimerCountdown">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r3TimerProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelTimer(3)">Cancel Countdown</button>
          </div>

          <!-- Live Cycle Progress -->
          <div class="live-prog-box" id="r3CycleLiveBox" style="display:none; background:rgba(0, 230, 118, 0.1); border-color:rgba(0,230,118,0.3);">
            <div class="live-prog-header">
              <span id="r3CyclePhase">🔁 Cycle Running</span>
              <span class="live-prog-val" id="r3CycleCountdown" style="color:var(--success);">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r3CycleProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelCycle(3)">Stop Cycle Loop</button>
          </div>

          <!-- Tab Navigation -->
          <div class="tab-nav">
            <button class="tab-btn active" onclick="switchTab(3, 'timer')">⏱️ Timer</button>
            <button class="tab-btn" onclick="switchTab(3, 'cycle')">🔁 Cycle</button>
            <button class="tab-btn" onclick="switchTab(3, 'sched')">📅 Schedule</button>
            <button class="tab-btn" onclick="switchTab(3, 'stats')">📊 Energy</button>
          </div>

          <!-- Tab 1: Timer (2 Inputs clearly explained) -->
          <div class="tab-pane active" id="r3TabTimer">
            <div class="form-group">
              <label class="form-label">1. Start Delay (Wait before turning ON):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r3DelayMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r3DelaySec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">Time to wait before turning ON. (Set 0 to turn ON immediately)</span>
            </div>
            <div class="form-group">
              <label class="form-label">2. ON Duration (Stay ON before turning OFF):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r3DurMin" placeholder="Mins" min="0" value="10">
                <input type="number" class="input-box" id="r3DurSec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">How long the relay stays ON before automatically turning OFF.</span>
            </div>
            <button class="btn btn-primary" onclick="setTimer(3)">Start Countdown Timer</button>
          </div>

          <!-- Tab 2: Cycle Automation -->
          <div class="tab-pane" id="r3TabCycle">
            <div class="form-group">
              <label class="form-label">ON Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r3CycOnMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r3CycOnSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">OFF Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r3CycOffMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r3CycOffSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">Total Cycles (0 = Infinite Loop):</label>
              <input type="number" class="input-box" id="r3CycTotal" placeholder="0 = Infinite Loop" min="0" value="0">
            </div>
            <button class="btn btn-primary" onclick="setCycle(3)">Start Cycle Automation</button>
          </div>

          <!-- Tab 3: Daily Schedule -->
          <div class="tab-pane" id="r3TabSched">
            <div class="form-group">
              <label class="form-label">Daily Turn ON Time (HH:MM):</label>
              <input type="time" class="input-box" id="r3SchedStart" value="08:00">
            </div>
            <div class="form-group">
              <label class="form-label">Daily Turn OFF Time (HH:MM):</label>
              <input type="time" class="input-box" id="r3SchedEnd" value="18:00">
            </div>
            <div class="form-group">
              <div style="display:flex; justify-content:space-between; align-items:center;">
                <label class="form-label">Active Weekdays:</label>
                <div class="day-presets">
                  <button type="button" class="preset-btn" onclick="selectDayPreset(3, 'all')">All</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(3, 'weekdays')">Mon-Fri</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(3, 'weekends')">Sat-Sun</button>
                </div>
              </div>
              <div class="weekday-bar" id="r3WeekdayBar">
                <div class="day-btn selected" data-day="0" onclick="toggleDay(3, 0)">Sun</div>
                <div class="day-btn selected" data-day="1" onclick="toggleDay(3, 1)">Mon</div>
                <div class="day-btn selected" data-day="2" onclick="toggleDay(3, 2)">Tue</div>
                <div class="day-btn selected" data-day="3" onclick="toggleDay(3, 3)">Wed</div>
                <div class="day-btn selected" data-day="4" onclick="toggleDay(3, 4)">Thu</div>
                <div class="day-btn selected" data-day="5" onclick="toggleDay(3, 5)">Fri</div>
                <div class="day-btn selected" data-day="6" onclick="toggleDay(3, 6)">Sat</div>
              </div>
            </div>
            <div class="input-row" style="align-items:center; justify-content:space-between; margin-top:4px;">
              <span style="font-size:12px; color:var(--text-sub); font-weight:700;">Enable Daily Schedule</span>
              <label class="toggle-switch">
                <input type="checkbox" id="r3SchedEn" onchange="setSchedule(3)">
                <span class="slider"></span>
              </label>
            </div>
          </div>

          <!-- Tab 4: Energy & Stats -->
          <div class="tab-pane" id="r3TabStats">
            <div class="stats-row">
              <div class="stat-item">
                <span class="telem-title">Total ON Runtime</span>
                <span class="stat-val" id="r3TotTime">0h 0m</span>
              </div>
              <div class="stat-item">
                <span class="telem-title">Est. Energy Consumed</span>
                <span class="stat-val" id="r3Energy">0.000 kWh</span>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- Relay 4 Dynamic Card -->
      <div class="relay-card" id="relayCard4">
        <div class="relay-head-bar" onclick="toggleAccordion(4)">
          <div class="relay-info-left">
            <div class="relay-ch-badge">R4</div>
            <div class="relay-title-group">
              <span class="relay-title" id="r4Name">Relay 4 (D21)</span>
              <span class="relay-active-pill" id="r4ActivePill">⏱️ Active</span>
            </div>
          </div>
          <div class="relay-head-right" onclick="event.stopPropagation()">
            <label class="toggle-switch">
              <input type="checkbox" id="r4Switch" onchange="toggleRelay(4, this.checked)">
              <span class="slider"></span>
            </label>
            <button class="expand-btn" onclick="toggleAccordion(4); event.stopPropagation();">
              <span>Settings</span>
              <span class="expand-caret">▼</span>
            </button>
          </div>
        </div>

        <div class="relay-body" id="r4Body">
          <!-- Live Timer Progress -->
          <div class="live-prog-box" id="r4TimerLiveBox" style="display:none;">
            <div class="live-prog-header">
              <span id="r4TimerPhase">Timer: Active</span>
              <span class="live-prog-val" id="r4TimerCountdown">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r4TimerProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelTimer(4)">Cancel Countdown</button>
          </div>

          <!-- Live Cycle Progress -->
          <div class="live-prog-box" id="r4CycleLiveBox" style="display:none; background:rgba(0, 230, 118, 0.1); border-color:rgba(0,230,118,0.3);">
            <div class="live-prog-header">
              <span id="r4CyclePhase">🔁 Cycle Running</span>
              <span class="live-prog-val" id="r4CycleCountdown" style="color:var(--success);">00:00</span>
            </div>
            <div class="progress-track">
              <div class="progress-fill" id="r4CycleProgress"></div>
            </div>
            <button class="btn btn-danger" style="padding:4px 8px; font-size:11px;" onclick="cancelCycle(4)">Stop Cycle Loop</button>
          </div>

          <!-- Tab Navigation -->
          <div class="tab-nav">
            <button class="tab-btn active" onclick="switchTab(4, 'timer')">⏱️ Timer</button>
            <button class="tab-btn" onclick="switchTab(4, 'cycle')">🔁 Cycle</button>
            <button class="tab-btn" onclick="switchTab(4, 'sched')">📅 Schedule</button>
            <button class="tab-btn" onclick="switchTab(4, 'stats')">📊 Energy</button>
          </div>

          <!-- Tab 1: Timer (2 Inputs clearly explained) -->
          <div class="tab-pane active" id="r4TabTimer">
            <div class="form-group">
              <label class="form-label">1. Start Delay (Wait before turning ON):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r4DelayMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r4DelaySec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">Time to wait before turning ON. (Set 0 to turn ON immediately)</span>
            </div>
            <div class="form-group">
              <label class="form-label">2. ON Duration (Stay ON before turning OFF):</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r4DurMin" placeholder="Mins" min="0" value="20">
                <input type="number" class="input-box" id="r4DurSec" placeholder="Secs" min="0" max="59" value="0">
              </div>
              <span class="form-hint">How long the relay stays ON before automatically turning OFF.</span>
            </div>
            <button class="btn btn-primary" onclick="setTimer(4)">Start Countdown Timer</button>
          </div>

          <!-- Tab 2: Cycle Automation -->
          <div class="tab-pane" id="r4TabCycle">
            <div class="form-group">
              <label class="form-label">ON Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r4CycOnMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r4CycOnSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">OFF Duration per Cycle:</label>
              <div class="input-row">
                <input type="number" class="input-box" id="r4CycOffMin" placeholder="Mins" min="0" value="0">
                <input type="number" class="input-box" id="r4CycOffSec" placeholder="Secs" min="1" value="30">
              </div>
            </div>
            <div class="form-group">
              <label class="form-label">Total Cycles (0 = Infinite Loop):</label>
              <input type="number" class="input-box" id="r4CycTotal" placeholder="0 = Infinite Loop" min="0" value="0">
            </div>
            <button class="btn btn-primary" onclick="setCycle(4)">Start Cycle Automation</button>
          </div>

          <!-- Tab 3: Daily Schedule -->
          <div class="tab-pane" id="r4TabSched">
            <div class="form-group">
              <label class="form-label">Daily Turn ON Time (HH:MM):</label>
              <input type="time" class="input-box" id="r4SchedStart" value="18:00">
            </div>
            <div class="form-group">
              <label class="form-label">Daily Turn OFF Time (HH:MM):</label>
              <input type="time" class="input-box" id="r4SchedEnd" value="22:00">
            </div>
            <div class="form-group">
              <div style="display:flex; justify-content:space-between; align-items:center;">
                <label class="form-label">Active Weekdays:</label>
                <div class="day-presets">
                  <button type="button" class="preset-btn" onclick="selectDayPreset(4, 'all')">All</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(4, 'weekdays')">Mon-Fri</button>
                  <button type="button" class="preset-btn" onclick="selectDayPreset(4, 'weekends')">Sat-Sun</button>
                </div>
              </div>
              <div class="weekday-bar" id="r4WeekdayBar">
                <div class="day-btn selected" data-day="0" onclick="toggleDay(4, 0)">Sun</div>
                <div class="day-btn selected" data-day="1" onclick="toggleDay(4, 1)">Mon</div>
                <div class="day-btn selected" data-day="2" onclick="toggleDay(4, 2)">Tue</div>
                <div class="day-btn selected" data-day="3" onclick="toggleDay(4, 3)">Wed</div>
                <div class="day-btn selected" data-day="4" onclick="toggleDay(4, 4)">Thu</div>
                <div class="day-btn selected" data-day="5" onclick="toggleDay(4, 5)">Fri</div>
                <div class="day-btn selected" data-day="6" onclick="toggleDay(4, 6)">Sat</div>
              </div>
            </div>
            <div class="input-row" style="align-items:center; justify-content:space-between; margin-top:4px;">
              <span style="font-size:12px; color:var(--text-sub); font-weight:700;">Enable Daily Schedule</span>
              <label class="toggle-switch">
                <input type="checkbox" id="r4SchedEn" onchange="setSchedule(4)">
                <span class="slider"></span>
              </label>
            </div>
          </div>

          <!-- Tab 4: Energy & Stats -->
          <div class="tab-pane" id="r4TabStats">
            <div class="stats-row">
              <div class="stat-item">
                <span class="telem-title">Total ON Runtime</span>
                <span class="stat-val" id="r4TotTime">0h 0m</span>
              </div>
              <div class="stat-item">
                <span class="telem-title">Est. Energy Consumed</span>
                <span class="stat-val" id="r4Energy">0.000 kWh</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Total Energy Consumption Card (Frontend Summary) -->
    <div class="energy-total-card" id="energySection">
      <div class="energy-total-header">
        <span class="energy-title">⚡ Total Energy Consumed</span>
        <span class="energy-val-badge" id="totalKwhBadge">0.000 kWh</span>
      </div>
      <div class="energy-subgrid">
        <div class="energy-subitem">
          <span class="energy-subtitle">Total Runtime (All 4 Relays)</span>
          <span class="energy-subval" id="totalRuntimeVal">0h 0m</span>
        </div>
        <div class="energy-subitem">
          <span class="energy-subtitle">Live Connected Power</span>
          <span class="energy-subval" id="totalLiveWattsVal" style="color:var(--success);">0.0 W</span>
        </div>
      </div>
    </div>

    <!-- BACKEND & SYSTEM SERVICES BANNER -->
    <div class="section-banner backend-banner" id="backendSection">
      <div class="section-banner-left">
        <span class="section-badge backend-badge">⚙️ BACKEND & SYSTEM ENGINE</span>
        <span class="section-banner-text">FreeRTOS Dual-Core Engine, Auto-Sleep & Telemetry</span>
      </div>
      <span class="section-pill backend-pill">Core 0 & Core 1</span>
    </div>

    <!-- Auto-Sleep Live Countdown & Low Power Saver Card -->
    <div class="sleep-countdown-card" id="powerSection">
      <div class="sleep-left">
        <div class="sleep-title">
          <span>🔋 Auto-Sleep Power Saver</span>
          <span class="sleep-countdown-val" id="sleepCountdownText">--:--</span>
        </div>
        <span class="sleep-sub" id="sleepStatusSub">Auto-sleep active. Inactivity will shut off Wi-Fi AP & pulse Blue LED smoothly.</span>
      </div>
      <label class="toggle-switch">
        <input type="checkbox" id="powerSaverSwitch" onchange="togglePowerSaver(this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <!-- FreeRTOS System Telemetry & Diagnostics -->
    <div class="telemetry-card" id="telemetrySection">
      <div style="display:flex; justify-content:space-between; align-items:center;">
        <div style="display:flex; align-items:center; gap:8px;">
          <span style="font-weight:800; font-size:13px; color:#fff;">📊 FreeRTOS System Telemetry</span>
          <span class="section-badge backend-badge" style="font-size:9px; padding:2px 6px;">BACKEND</span>
        </div>
        <button class="btn btn-primary" style="padding:4px 10px; font-size:11px;" onclick="openSettingsModal()">⚙️ Config</button>
      </div>

      <div class="telemetry-grid">
        <div class="telem-box">
          <span class="telem-title">ESP32 Chip Temp</span>
          <span class="telem-val" id="telemTemp">-- °C</span>
        </div>
        <div class="telem-box">
          <span class="telem-title">Free Heap RAM</span>
          <span class="telem-val" id="telemHeap">-- KB</span>
        </div>
        <div class="telem-box">
          <span class="telem-title">System Uptime</span>
          <span class="telem-val" id="telemUptime">--</span>
        </div>
        <div class="telem-box">
          <span class="telem-title">Connected Stations</span>
          <span class="telem-val" id="telemClients">1</span>
        </div>
        <div class="telem-box">
          <span class="telem-title">Core 1 RTOS Ticks</span>
          <span class="telem-val" id="telemRelayTicks" style="color:var(--primary);">--</span>
        </div>
        <div class="telem-box">
          <span class="telem-title">Core 0 RTOS Ticks</span>
          <span class="telem-val" id="telemPowerTicks" style="color:var(--success);">--</span>
        </div>
      </div>

      <!-- Real-Time Activity Log -->
      <div style="display:flex; flex-direction:column; gap:6px; margin-top:2px;">
        <span style="font-size:11px; font-weight:700; color:var(--text-sub);">📜 Real-Time System Event Stream:</span>
        <div class="event-log-box" id="eventLogContainer">
          <div class="event-entry"><span class="event-time">--:--:--</span><span>Connecting to FreeRTOS system...</span></div>
        </div>
      </div>

      <!-- Master Emergency Kill Switch -->
      <button class="master-emergency-btn" onclick="killAllRelays()">🚨 EMERGENCY ALL OFF</button>
    </div>

    <!-- System Footer -->
    <footer class="footer-card">
      <div class="footer-top">
        <div>
          <div style="font-weight:800; color:#fff; font-size:12px;">ESP32 RTOS 4-Channel Control System</div>
          <div style="font-size:10px; color:var(--text-sub);">Relay 1: <b>D5</b> &bull; Relay 2: <b>D18</b> &bull; Relay 3: <b>D19</b> &bull; Relay 4: <b>D21</b></div>
        </div>
        <div style="display:flex; gap:6px; align-items:center;">
          <span class="version-badge" id="footerFwVer">FW: v2.0.0-OTA</span>
          <span class="version-badge" id="footerTgStatus" style="border-color:var(--tg-color); color:var(--tg-color);">Telegram: Off</span>
        </div>
      </div>
    </footer>
  </div>

  <!-- Telegram Bot Hub Modal -->
  <div class="modal-overlay" id="telegramModal">
    <div class="modal">
      <div class="modal-header">
        <div class="modal-title">
          <span>✈️ Telegram Bot & Alerts Hub</span>
        </div>
        <button class="close-btn" onclick="closeTelegramModal()">&times;</button>
      </div>

      <div style="background:rgba(41, 182, 246, 0.12); border:1px solid rgba(41, 182, 246, 0.35); border-radius:12px; padding:12px; display:flex; flex-direction:column; gap:4px;">
        <div style="font-weight:800; color:#fff; font-size:13px;">📶 Internet Connection via Wi-Fi Station</div>
        <div style="font-size:11px; color:var(--text-sub); line-height:1.4;">
          The ESP32 connects to your home/office Wi-Fi router (STA mode) to send alerts and receive commands from Telegram, while continuing its local AP server!
        </div>
        <div style="margin-top:4px; font-size:11px; font-weight:700;">
          Status: <span id="modalTgStatusText" style="color:var(--warning);">Connecting / Offline</span>
        </div>
      </div>

      <div class="form-group">
        <div style="display:flex; justify-content:space-between; align-items:center;">
          <label class="form-label" style="color:var(--tg-color);">Enable Telegram Bot Integration</label>
          <label class="toggle-switch">
            <input type="checkbox" id="tgEnableSwitch">
            <span class="slider"></span>
          </label>
        </div>
      </div>

      <!-- Telegram Bot Token & Chat ID -->
      <div class="form-group">
        <label class="form-label">Telegram Bot Token (from @BotFather):</label>
        <input type="text" class="input-box" id="tgBotTokenInput" placeholder="123456789:ABCdefGhIJKlmNoPQRstuvwxYZ">
        <span class="form-hint">Create a bot with @BotFather on Telegram and paste the HTTP API token here.</span>
      </div>

      <div class="form-group">
        <label class="form-label">Target Chat / User ID (from @userinfobot):</label>
        <input type="text" class="input-box" id="tgChatIdInput" placeholder="e.g. 123456789">
        <span class="form-hint">Send any message to @userinfobot on Telegram to discover your numeric Chat ID.</span>
      </div>

      <!-- Home Wi-Fi STA Credentials -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:var(--primary);">Home / Office Wi-Fi (For Internet Access):</label>
        <input type="text" class="input-box" id="tgStaSsidInput" placeholder="Wi-Fi Router SSID" style="margin-bottom:6px;">
        <input type="password" class="input-box" id="tgStaPassInput" placeholder="Wi-Fi Router Password">
      </div>

      <!-- Notification Triggers -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label">Instant Notification Triggers:</label>
        <label style="display:flex; align-items:center; gap:8px; font-size:12px; color:#fff; cursor:pointer; margin-top:2px;">
          <input type="checkbox" id="tgNotifyToggle" checked> Send message when Relay is switched ON/OFF
        </label>
        <label style="display:flex; align-items:center; gap:8px; font-size:12px; color:#fff; cursor:pointer; margin-top:4px;">
          <input type="checkbox" id="tgNotifyTimer" checked> Send message when Countdown Timer completes
        </label>
        <label style="display:flex; align-items:center; gap:8px; font-size:12px; color:#fff; cursor:pointer; margin-top:4px;">
          <input type="checkbox" id="tgNotifySchedule" checked> Send message when Daily Schedule triggers
        </label>
      </div>

      <!-- Action Buttons -->
      <div style="display:flex; gap:8px; margin-top:4px;">
        <button class="btn btn-tg" style="flex:1;" onclick="sendTelegramTest()">🚀 Send Test Message</button>
        <button class="btn btn-success" style="flex:1;" onclick="saveTelegramConfig()">💾 Save Telegram Settings</button>
      </div>

      <!-- Command Cheatsheet -->
      <div style="background:var(--card-inner-bg); border:1px solid var(--card-border); border-radius:10px; padding:10px 12px; font-size:11px; color:var(--text-sub); display:flex; flex-direction:column; gap:4px;">
        <b style="color:#fff;">💡 Telegram Commands you can send to your Bot:</b>
        <div>• <code>/status</code> - Query live status of all 4 relays, temperature & energy</div>
        <div>• <code>/r1_on</code>, <code>/r1_off</code>, <code>/r1_toggle</code> - Control Relay 1</div>
        <div>• <code>/r2_on</code>, <code>/r2_off</code>, <code>/r2_toggle</code> - Control Relay 2</div>
        <div>• <code>/r3_on</code>, <code>/r3_off</code>, <code>/r3_toggle</code> - Control Relay 3</div>
        <div>• <code>/r4_on</code>, <code>/r4_off</code>, <code>/r4_toggle</code> - Control Relay 4</div>
        <div>• <code>/all_on</code>, <code>/all_off</code> - Master All ON / Emergency ALL OFF</div>
      </div>
    </div>
  </div>

  <!-- Settings Modal -->
  <div class="modal-overlay" id="settingsModal">
    <div class="modal">
      <div class="modal-header">
        <div class="modal-title">⚙️ System & Power Settings</div>
        <button class="close-btn" onclick="closeSettingsModal()">&times;</button>
      </div>

      <!-- Permanent Stay-On Preference -->
      <div class="form-group">
        <label class="form-label" style="color:var(--primary);">⚡ Permanent Always-On Setting</label>
        <label style="display:flex; align-items:center; gap:10px; font-size:13px; color:#fff; cursor:pointer;">
          <input type="checkbox" id="modalPermanentStayOn" style="width:18px; height:18px;">
          <span>Keep AP & System running 24/7 permanently (Disable sleep)</span>
        </label>
        <span class="form-hint">Stored permanently in Flash memory across power reboots.</span>
      </div>

      <!-- Manual Time Setting -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:var(--primary);">🕒 Manual Date & Time Adjustment</label>
        <input type="datetime-local" class="input-box" id="manualDateTimeInput">
        <button class="btn btn-primary" style="margin-top:6px;" onclick="saveManualTime()">Set ESP32 Time</button>
      </div>

      <!-- Low Power Settings -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:var(--warning);">🔋 Auto-Sleep Timing Cycles</label>
        <div class="input-row">
          <div style="flex:1;">
            <span class="form-label" style="font-size:11px;">Inactivity Timeout (Mins):</span>
            <input type="number" class="input-box" id="lpWakeMin" value="2" min="1" max="30">
          </div>
          <div style="flex:1;">
            <span class="form-label" style="font-size:11px;">AP Sleep Interval (Mins):</span>
            <input type="number" class="input-box" id="lpSleepMin" value="5" min="1" max="120">
          </div>
        </div>
      </div>

      <!-- Channels Customization -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:#fff;">🏷️ Channel 1 Customization (D5)</label>
        <input type="text" class="input-box" id="cfgR1Name" placeholder="Relay 1 Name" value="Relay 1">
        <div class="input-row" style="margin-top:4px;">
          <input type="number" class="input-box" id="cfgR1Watts" placeholder="Watts (e.g. 100)" value="100">
          <select class="input-box" id="cfgR1PwrOn">
            <option value="0">Boot: OFF</option>
            <option value="1">Boot: ON</option>
            <option value="2">Boot: Restore Last</option>
          </select>
        </div>
      </div>

      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:#fff;">🏷️ Channel 2 Customization (D18)</label>
        <input type="text" class="input-box" id="cfgR2Name" placeholder="Relay 2 Name" value="Relay 2">
        <div class="input-row" style="margin-top:4px;">
          <input type="number" class="input-box" id="cfgR2Watts" placeholder="Watts (e.g. 100)" value="100">
          <select class="input-box" id="cfgR2PwrOn">
            <option value="0">Boot: OFF</option>
            <option value="1">Boot: ON</option>
            <option value="2">Boot: Restore Last</option>
          </select>
        </div>
      </div>

      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:#fff;">🏷️ Channel 3 Customization (D19)</label>
        <input type="text" class="input-box" id="cfgR3Name" placeholder="Relay 3 Name" value="Relay 3">
        <div class="input-row" style="margin-top:4px;">
          <input type="number" class="input-box" id="cfgR3Watts" placeholder="Watts (e.g. 100)" value="100">
          <select class="input-box" id="cfgR3PwrOn">
            <option value="0">Boot: OFF</option>
            <option value="1">Boot: ON</option>
            <option value="2">Boot: Restore Last</option>
          </select>
        </div>
      </div>

      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:10px;">
        <label class="form-label" style="color:#fff;">🏷️ Channel 4 Customization (D21)</label>
        <input type="text" class="input-box" id="cfgR4Name" placeholder="Relay 4 Name" value="Relay 4">
        <div class="input-row" style="margin-top:4px;">
          <input type="number" class="input-box" id="cfgR4Watts" placeholder="Watts (e.g. 100)" value="100">
          <select class="input-box" id="cfgR4PwrOn">
            <option value="0">Boot: OFF</option>
            <option value="1">Boot: ON</option>
            <option value="2">Boot: Restore Last</option>
          </select>
        </div>
      </div>

      <!-- OTA Upload via Port 80 & Port 500 -->
      <div class="form-group" style="border-top:1px solid var(--card-border); padding-top:12px;">
        <label class="form-label" style="color:var(--primary);">⚡ OTA Firmware Flashing</label>
        <div style="background:var(--card-inner-bg); border:1px solid var(--card-border); border-radius:10px; padding:10px; display:flex; flex-direction:column; gap:6px;">
          <div style="display:flex; justify-content:space-between; align-items:center;">
            <span style="font-size:12px; font-weight:700; color:#fff;">Port 500 Dedicated Server</span>
            <label class="toggle-switch">
              <input type="checkbox" id="port500EnSwitch" onchange="togglePort500(this.checked)">
              <span class="slider"></span>
            </label>
          </div>
          <span style="font-size:10px; color:var(--text-sub);">When enabled, access dedicated flasher at <a id="port500Link" href="http://192.168.4.1:500" target="_blank" style="color:var(--primary); text-decoration:underline; display:none;">http://192.168.4.1:500</a></span>
        </div>

        <div style="background:var(--card-inner-bg); border:1px solid var(--card-border); border-radius:10px; padding:10px; margin-top:6px; display:flex; flex-direction:column; gap:6px;">
          <span style="font-size:11px; font-weight:700; color:#fff;">Upload .bin Firmware via Port 80:</span>
          <input type="file" id="port80FileInput" accept=".bin" class="input-box" style="padding:5px; font-size:11px;" onchange="onPort80FileSelected()">
          <div id="port80FileInfo" style="display:none; font-size:10px; color:var(--primary); font-family:monospace;"></div>
          <button type="button" class="btn btn-primary" id="port80UploadBtn" style="padding:7px;" onclick="uploadFirmwarePort80()" disabled>🚀 Upload & Flash Firmware</button>
        </div>
      </div>

      <button class="btn btn-success" style="margin-top:6px;" onclick="saveAllSettings()">Save All Settings to Memory</button>
    </div>
  </div>

  <div id="toast">Notification</div>

  <script>
    let isModalOpen = false;
    let activeAccordionId = null; // Track which relay card is expanded
    const rDays = { 1: 0xFF, 2: 0xFF, 3: 0xFF, 4: 0xFF };
    const relayStates = { 1: false, 2: false, 3: false, 4: false };

    // Toast Notification
    function showToast(msg) {
      const toast = document.getElementById('toast');
      toast.innerText = msg;
      toast.classList.add('show');
      setTimeout(() => toast.classList.remove('show'), 2500);
    }

    // Sidenav Toggle
    function toggleSidenav(open) {
      const drawer = document.getElementById('sidenavDrawer');
      const overlay = document.getElementById('sidenavOverlay');
      if (open) {
        drawer.classList.add('open');
        overlay.classList.add('open');
      } else {
        drawer.classList.remove('open');
        overlay.classList.remove('open');
      }
    }

    // Modal Handlers
    function openSettingsModal() {
      document.getElementById('settingsModal').classList.add('open');
      isModalOpen = true;
    }
    function closeSettingsModal() {
      document.getElementById('settingsModal').classList.remove('open');
      isModalOpen = false;
    }
    function openTelegramModal() {
      document.getElementById('telegramModal').classList.add('open');
      isModalOpen = true;
    }
    function closeTelegramModal() {
      document.getElementById('telegramModal').classList.remove('open');
      isModalOpen = false;
    }

    // Dynamic Accordion Handler (Auto-minimizes other relays when one is opened)
    function toggleAccordion(relayId) {
      const targetCard = document.getElementById(`relayCard${relayId}`);
      const isAlreadyOpen = targetCard.classList.contains('open');

      // Close all 4 relay cards first
      for (let i = 1; i <= 4; i++) {
        const c = document.getElementById(`relayCard${i}`);
        if (c) c.classList.remove('open');
      }

      // If it wasn't open, open it now (if it was open, it is now minimized)
      if (!isAlreadyOpen) {
        targetCard.classList.add('open');
        activeAccordionId = relayId;
      } else {
        activeAccordionId = null;
      }
    }

    // Tab Switching inside Relay Card
    function switchTab(relayId, tabName) {
      const card = document.getElementById(`relayCard${relayId}`);
      card.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
      card.querySelectorAll('.tab-pane').forEach(pane => pane.classList.remove('active'));

      event.target.classList.add('active');
      if (tabName === 'timer') card.querySelector(`#r${relayId}TabTimer`).classList.add('active');
      if (tabName === 'cycle') card.querySelector(`#r${relayId}TabCycle`).classList.add('active');
      if (tabName === 'sched') card.querySelector(`#r${relayId}TabSched`).classList.add('active');
      if (tabName === 'stats') card.querySelector(`#r${relayId}TabStats`).classList.add('active');
    }

    // Quick Shortcut Relay Toggle (Screen Shortcut)
    async function quickToggleRelay(id) {
      const newState = !relayStates[id];
      await toggleRelay(id, newState);
    }

    // Relay Manual Toggle
    async function toggleRelay(id, state) {
      try {
        const res = await fetch('/api/relay', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&state=${state ? 1 : 0}`
        });
        if (res.ok) {
          showToast(`Relay ${id} switched ${state ? 'ON' : 'OFF'}`);
          relayStates[id] = state;
          updateRelayVisuals(id, state);
          fetchStatus();
        }
      } catch (err) {
        showToast('Error sending command');
      }
    }

    function updateRelayVisuals(id, state) {
      const card = document.getElementById(`relayCard${id}`);
      const sw = document.getElementById(`r${id}Switch`);
      const scBtn = document.getElementById(`scR${id}`);
      const scBadge = document.getElementById(`scR${id}Badge`);

      if (sw) sw.checked = state;
      if (card) {
        if (state) card.classList.add('active');
        else card.classList.remove('active');
      }
      if (scBtn && scBadge) {
        if (state) {
          scBtn.classList.add('on');
          scBadge.className = 'sc-badge on';
          scBadge.innerText = 'ON';
        } else {
          scBtn.classList.remove('on');
          scBadge.className = 'sc-badge off';
          scBadge.innerText = 'OFF';
        }
      }
    }

    // Master All ON / All OFF
    async function masterAllOn() {
      try {
        const res = await fetch('/api/all_on', { method: 'POST' });
        if (res.ok) {
          showToast('All Relays Switched ON!');
          for (let i = 1; i <= 4; i++) updateRelayVisuals(i, true);
          fetchStatus();
        }
      } catch (err) {
        showToast('Failed to switch all ON');
      }
    }

    async function killAllRelays() {
      if (confirm('Turn OFF all 4 relays immediately?')) {
        try {
          const res = await fetch('/api/all_off', { method: 'POST' });
          if (res.ok) {
            showToast('All Relays Forced OFF!');
            for (let i = 1; i <= 4; i++) updateRelayVisuals(i, false);
            fetchStatus();
          }
        } catch(err) {
          showToast('Error triggering Emergency Off');
        }
      }
    }

    // Countdown Timer Start
    async function setTimer(id) {
      const delayMin = parseInt(document.getElementById(`r${id}DelayMin`).value) || 0;
      const delaySec = parseInt(document.getElementById(`r${id}DelaySec`).value) || 0;
      const durMin = parseInt(document.getElementById(`r${id}DurMin`).value) || 0;
      const durSec = parseInt(document.getElementById(`r${id}DurSec`).value) || 0;

      const totalDelay = delayMin * 60 + delaySec;
      const totalDur = durMin * 60 + durSec;

      if (totalDelay === 0 && totalDur === 0) {
        showToast('Please specify a valid delay or duration');
        return;
      }

      try {
        const res = await fetch('/api/timer', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&start_delay=${totalDelay}&duration=${totalDur}&enable=1`
        });
        if (res.ok) {
          showToast(`Relay ${id} Countdown Timer Started!`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Error starting timer');
      }
    }

    async function cancelTimer(id) {
      try {
        const res = await fetch('/api/timer', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&enable=0`
        });
        if (res.ok) {
          showToast(`Relay ${id} Countdown Cancelled`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Error cancelling timer');
      }
    }

    // Cyclic Automation
    async function setCycle(id) {
      const onMin = parseInt(document.getElementById(`r${id}CycOnMin`).value) || 0;
      const onSec = parseInt(document.getElementById(`r${id}CycOnSec`).value) || 0;
      const offMin = parseInt(document.getElementById(`r${id}CycOffMin`).value) || 0;
      const offSec = parseInt(document.getElementById(`r${id}CycOffSec`).value) || 0;
      const totalCycles = parseInt(document.getElementById(`r${id}CycTotal`).value) || 0;

      const totalOnSec = onMin * 60 + onSec;
      const totalOffSec = offMin * 60 + offSec;

      if (totalOnSec <= 0 || totalOffSec <= 0) {
        showToast('Specify valid ON & OFF times');
        return;
      }

      try {
        const res = await fetch('/api/cycle', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&on_sec=${totalOnSec}&off_sec=${totalOffSec}&cycles=${totalCycles}&enable=1`
        });
        if (res.ok) {
          showToast(`Relay ${id} Cycle Started!`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Error starting cycle');
      }
    }

    async function cancelCycle(id) {
      try {
        const res = await fetch('/api/cycle', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&enable=0`
        });
        if (res.ok) {
          showToast(`Relay ${id} Cycle Stopped`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Error stopping cycle');
      }
    }

    // Weekday Schedule Selection
    function toggleDay(relayId, dayIdx) {
      if (rDays[relayId] === 0xFF) rDays[relayId] = 0x7F;
      rDays[relayId] ^= (1 << dayIdx);
      updateDayUi(relayId);
      setSchedule(relayId);
    }

    function selectDayPreset(relayId, preset) {
      if (preset === 'all') rDays[relayId] = 0x7F;
      else if (preset === 'weekdays') rDays[relayId] = 0x3E;
      else if (preset === 'weekends') rDays[relayId] = 0x41;
      updateDayUi(relayId);
      setSchedule(relayId);
    }

    function updateDayUi(relayId) {
      const bar = document.getElementById(`r${relayId}WeekdayBar`);
      if (!bar) return;
      const mask = rDays[relayId];
      bar.querySelectorAll('.day-btn').forEach(btn => {
        const d = parseInt(btn.getAttribute('data-day'));
        if ((mask & (1 << d)) || mask === 0xFF) btn.classList.add('selected');
        else btn.classList.remove('selected');
      });
    }

    async function setSchedule(id) {
      const isEn = document.getElementById(`r${id}SchedEn`).checked;
      const startVal = document.getElementById(`r${id}SchedStart`).value.split(':');
      const endVal = document.getElementById(`r${id}SchedEnd`).value.split(':');

      const startH = parseInt(startVal[0]) || 0;
      const startM = parseInt(startVal[1]) || 0;
      const endH = parseInt(endVal[0]) || 0;
      const endM = parseInt(endVal[1]) || 0;
      const mask = rDays[id] !== undefined ? rDays[id] : 0xFF;

      try {
        const res = await fetch('/api/schedule', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `id=${id}&start_h=${startH}&start_m=${startM}&end_h=${endH}&end_m=${endM}&days=${mask}&enable=${isEn ? 1 : 0}`
        });
        if (res.ok) {
          showToast(`Relay ${id} Schedule ${isEn ? 'Enabled' : 'Disabled'}`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Error saving schedule');
      }
    }

    // Phone Time Sync
    async function syncPhoneTime() {
      const now = new Date();
      const epochSec = Math.floor(now.getTime() / 1000);
      const tzOffset = -now.getTimezoneOffset();

      try {
        const res = await fetch('/api/time/sync', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `epoch=${epochSec}&tz_offset=${tzOffset}`
        });
        if (res.ok) {
          showToast('Time Synced with Phone!');
          let h = now.getHours();
          const ampm = h >= 12 ? 'PM' : 'AM';
          h = h % 12;
          if (h === 0) h = 12;
          const m = String(now.getMinutes()).padStart(2, '0');
          const s = String(now.getSeconds()).padStart(2, '0');
          document.getElementById('liveClock').innerText = `${String(h).padStart(2, '0')}:${m}:${s} ${ampm}`;
          fetchStatus();
        }
      } catch(err) {
        showToast('Time sync failed');
      }
    }

    async function saveManualTime() {
      const val = document.getElementById('manualDateTimeInput').value;
      if (!val) { showToast('Select valid date & time'); return; }
      const d = new Date(val);

      try {
        const res = await fetch('/api/time/manual', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `year=${d.getFullYear()}&month=${d.getMonth()+1}&day=${d.getDate()}&hour=${d.getHours()}&min=${d.getMinutes()}&sec=${d.getSeconds()}`
        });
        if (res.ok) {
          showToast('Manual Time Saved!');
          closeSettingsModal();
          fetchStatus();
        }
      } catch(err) {
        showToast('Failed to set manual time');
      }
    }

    // Auto-Sleep Low Power Saver Mode Toggle
    async function togglePowerSaver(enable) {
      try {
        const res = await fetch('/api/power', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `enable=${enable ? 1 : 0}`
        });
        if (res.ok) {
          showToast(enable ? '🔋 Auto-Sleep Power Saver Enabled' : '⚡ 24/7 Continuous Mode Active');
          fetchStatus();
        }
      } catch(err) {
        showToast('Error changing power setting');
      }
    }

    async function togglePermanentStayOn(stayOn) {
      await togglePowerSaver(!stayOn);
    }

    // Telegram Configuration & Test
    async function saveTelegramConfig() {
      const en = document.getElementById('tgEnableSwitch').checked ? 1 : 0;
      const tok = document.getElementById('tgBotTokenInput').value.trim();
      const cid = document.getElementById('tgChatIdInput').value.trim();
      const ssid = document.getElementById('tgStaSsidInput').value.trim();
      const pass = document.getElementById('tgStaPassInput').value.trim();
      const nTog = document.getElementById('tgNotifyToggle').checked ? 1 : 0;
      const nTim = document.getElementById('tgNotifyTimer').checked ? 1 : 0;
      const nSch = document.getElementById('tgNotifySchedule').checked ? 1 : 0;

      try {
        const res = await fetch('/api/telegram/config', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `enable=${en}&token=${encodeURIComponent(tok)}&chat_id=${encodeURIComponent(cid)}&sta_ssid=${encodeURIComponent(ssid)}&sta_pass=${encodeURIComponent(pass)}&n_tog=${nTog}&n_tim=${nTim}&n_sch=${nSch}`
        });
        if (res.ok) {
          showToast('Telegram Settings Saved Permanently!');
          closeTelegramModal();
          fetchStatus();
        }
      } catch (err) {
        showToast('Failed to save Telegram config');
      }
    }

    async function sendTelegramTest() {
      showToast('Sending test message to Telegram...');
      try {
        const res = await fetch('/api/telegram/test', { method: 'POST' });
        if (res.ok) {
          showToast('🎉 Test Message Sent to Telegram!');
        } else {
          showToast('❌ Test message failed. Check Wi-Fi & Bot token.');
        }
      } catch (err) {
        showToast('Network error sending test message');
      }
    }

    // Save All System Settings
    async function saveAllSettings() {
      const stayOn = document.getElementById('modalPermanentStayOn').checked;
      const sleepMin = parseInt(document.getElementById('lpSleepMin').value) || 5;
      const wakeMin = parseInt(document.getElementById('lpWakeMin').value) || 2;

      let bodyParams = [];
      for (let i = 1; i <= 4; i++) {
        const name = document.getElementById(`cfgR${i}Name`).value;
        const watts = parseFloat(document.getElementById(`cfgR${i}Watts`).value) || 100;
        const pwrOn = parseInt(document.getElementById(`cfgR${i}PwrOn`).value) || 0;
        bodyParams.push(`r${i}_name=${encodeURIComponent(name)}&r${i}_watts=${watts}&r${i}_actlow=1&r${i}_pwron=${pwrOn}`);
      }

      try {
        await fetch('/api/power', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `enable=${stayOn ? 0 : 1}&stay_on=${stayOn ? 1 : 0}&sleep_min=${sleepMin}&wake_min=${wakeMin}`
        });

        await fetch('/api/settings', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: bodyParams.join('&')
        });

        showToast('All Settings Permanently Saved!');
        closeSettingsModal();
        fetchStatus();
      } catch(err) {
        showToast('Failed saving settings');
      }
    }

    function formatSecs(secs) {
      const m = Math.floor(secs / 60);
      const s = secs % 60;
      return `${String(m).padStart(2,'0')}:${String(s).padStart(2,'0')}`;
    }

    function formatHoursMin(totalSecs) {
      const h = Math.floor(totalSecs / 3600);
      const m = Math.floor((totalSecs % 3600) / 60);
      return `${h}h ${m}m`;
    }

    // Status Polling Loop
    async function fetchStatus() {
      try {
        const res = await fetch('/api/status');
        if (!res.ok) return;
        const data = await res.json();

        // Clock & Date
        document.getElementById('liveClock').innerText = data.time.clock;
        document.getElementById('liveDate').innerText = data.time.date + (data.time.synced ? ' (Synced)' : ' (Not Synced)');

        // Power Saver & Sleep Countdown Display
        const psSwitch = document.getElementById('powerSaverSwitch');
        if (psSwitch) psSwitch.checked = data.power.enabled;
        if (!isModalOpen) {
          const mStay = document.getElementById('modalPermanentStayOn');
          if (mStay) mStay.checked = data.power.stay_on;
        }

        const cdElem = document.getElementById('sleepCountdownText');
        const subElem = document.getElementById('sleepStatusSub');
        if (data.power.sleeping) {
          if (cdElem) cdElem.innerText = formatSecs(data.power.next_state_sec);
          if (subElem) subElem.innerHTML = '💤 <b>AP Sleeping (Blue LED Smooth Breathing)</b> &bull; Discovery wake window in:';
        } else if (data.power.enabled) {
          if (cdElem) cdElem.innerText = formatSecs(data.power.next_state_sec);
          if (subElem) subElem.innerHTML = '🔋 <b>Power Saver Active (Blue LED pulses in sleep):</b> Auto-sleeps in:';
        } else {
          if (cdElem) cdElem.innerText = '24/7 ON';
          if (subElem) subElem.innerHTML = '⚡ <b>Always-On Mode:</b> Auto-sleep disabled. System runs 24/7 continuously.';
        }

        let totalSecs = 0;
        let totalKwhAcc = 0;
        let liveWatts = 0;

        // Process all 4 channels dynamically
        for (let i = 1; i <= 4; i++) {
          const r = data[`r${i}`];
          if (!r) continue;

          relayStates[i] = r.state;
          totalSecs += (r.total_sec || 0);
          const rKwh = ((r.total_sec / 3600) * (r.watts / 1000));
          totalKwhAcc += rKwh;
          if (r.state) liveWatts += (r.watts || 100);

          // Update Name
          const rNameElem = document.getElementById(`r${i}Name`);
          if (rNameElem) rNameElem.innerText = r.name;
          const scNameElem = document.getElementById(`scR${i}Name`);
          if (scNameElem) scNameElem.innerText = r.name;

          // Update Visuals (Glowing border, switches, shortcut button)
          updateRelayVisuals(i, r.state);

          // Active Pill in Header
          const actPill = document.getElementById(`r${i}ActivePill`);
          if (actPill) {
            if (r.timer && r.timer.active) {
              actPill.style.display = 'inline-flex';
              actPill.innerText = `⏱️ ${formatSecs(r.timer.remaining)}`;
            } else if (r.cycle && r.cycle.active) {
              actPill.style.display = 'inline-flex';
              actPill.innerText = `🔁 Cyc ${r.cycle.current_cycle+1}`;
            } else if (r.schedule && r.schedule.enabled) {
              actPill.style.display = 'inline-flex';
              actPill.innerText = '📅 Sched';
            } else {
              actPill.style.display = 'none';
            }
          }

          // Countdown Timer Info inside body
          const tBox = document.getElementById(`r${i}TimerLiveBox`);
          if (tBox) {
            if (r.timer && r.timer.active) {
              tBox.style.display = 'flex';
              document.getElementById(`r${i}TimerPhase`).innerText = r.timer.in_delay ? 'Delay Phase (Turn ON in):' : 'Active Phase (Turn OFF in):';
              document.getElementById(`r${i}TimerCountdown`).innerText = formatSecs(r.timer.remaining);
              const totalSpan = r.timer.in_delay ? r.timer.delay_sec : r.timer.dur_sec;
              const pct = totalSpan > 0 ? ((totalSpan - r.timer.remaining) / totalSpan) * 100 : 100;
              document.getElementById(`r${i}TimerProgress`).style.width = pct + '%';
            } else {
              tBox.style.display = 'none';
            }
          }

          // Cycle Automation Info
          const cBox = document.getElementById(`r${i}CycleLiveBox`);
          if (cBox) {
            if (r.cycle && r.cycle.active) {
              cBox.style.display = 'flex';
              const cycleLabel = r.cycle.total_cycles > 0 ? `Cycle ${r.cycle.current_cycle+1}/${r.cycle.total_cycles}` : `Cycle #${r.cycle.current_cycle+1} (Infinite)`;
              document.getElementById(`r${i}CyclePhase`).innerText = `🔁 ${cycleLabel} [${r.cycle.in_on ? 'ON Phase' : 'OFF Phase'}]`;
              document.getElementById(`r${i}CycleCountdown`).innerText = formatSecs(r.cycle.remaining);
              const phaseSpan = r.cycle.in_on ? r.cycle.on_sec : r.cycle.off_sec;
              const pct = phaseSpan > 0 ? ((phaseSpan - r.cycle.remaining) / phaseSpan) * 100 : 100;
              document.getElementById(`r${i}CycleProgress`).style.width = pct + '%';
            } else {
              cBox.style.display = 'none';
            }
          }

          // Schedule & Stats
          if (!isModalOpen) {
            const sEn = document.getElementById(`r${i}SchedEn`);
            if (sEn && r.schedule) {
              sEn.checked = r.schedule.enabled;
              document.getElementById(`r${i}SchedStart`).value = `${String(r.schedule.start_h).padStart(2,'0')}:${String(r.schedule.start_m).padStart(2,'0')}`;
              document.getElementById(`r${i}SchedEnd`).value = `${String(r.schedule.end_h).padStart(2,'0')}:${String(r.schedule.end_m).padStart(2,'0')}`;
              if (r.schedule.days !== undefined) {
                rDays[i] = r.schedule.days;
                updateDayUi(i);
              }
            }
          }
          const totTimeElem = document.getElementById(`r${i}TotTime`);
          if (totTimeElem) totTimeElem.innerText = formatHoursMin(r.total_sec);
          const nrgElem = document.getElementById(`r${i}Energy`);
          if (nrgElem) nrgElem.innerText = `${rKwh.toFixed(3)} kWh`;
        }

        // Combined Energy Metrics
        document.getElementById('totalKwhBadge').innerText = `${totalKwhAcc.toFixed(3)} kWh`;
        document.getElementById('totalRuntimeVal').innerText = formatHoursMin(totalSecs);
        document.getElementById('totalLiveWattsVal').innerText = `${liveWatts.toFixed(1)} W`;

        // Telemetry
        document.getElementById('telemTemp').innerText = `${data.telem.temp_c.toFixed(1)} °C`;
        document.getElementById('telemHeap').innerText = `${Math.round(data.telem.free_heap / 1024)} KB`;
        const ut_h = Math.floor(data.telem.uptime_sec / 3600);
        const ut_m = Math.floor((data.telem.uptime_sec % 3600) / 60);
        document.getElementById('telemUptime').innerText = `${ut_h}h ${ut_m}m`;
        document.getElementById('telemClients').innerText = data.telem.clients;
        if (data.telem.rtos_relay_ticks !== undefined) {
          document.getElementById('telemRelayTicks').innerText = data.telem.rtos_relay_ticks;
          document.getElementById('telemPowerTicks').innerText = data.telem.rtos_power_ticks;
        }

        // Sidenav telemetry
        const sTemp = document.getElementById('sideTemp');
        if (sTemp) sTemp.innerText = `${data.telem.temp_c.toFixed(1)} °C`;
        const sHeap = document.getElementById('sideHeap');
        if (sHeap) sHeap.innerText = `${Math.round(data.telem.free_heap / 1024)} KB`;
        const sUptime = document.getElementById('sideUptime');
        if (sUptime) sUptime.innerText = `${ut_h}h ${ut_m}m`;

        // Telegram Bot Status
        if (data.tg) {
          const tgDot = document.getElementById('tgHeaderDot');
          const tgFoot = document.getElementById('footerTgStatus');
          const tgModalStatus = document.getElementById('modalTgStatusText');
          
          if (data.tg.connected) {
            if (tgDot) tgDot.className = 'tg-dot online';
            if (tgFoot) { tgFoot.innerText = 'Telegram: Online'; tgFoot.style.color = 'var(--success)'; tgFoot.style.borderColor = 'var(--success)'; }
            if (tgModalStatus) { tgModalStatus.innerText = '🟢 Wi-Fi Connected & Online (Ready to Send/Receive)'; tgModalStatus.style.color = 'var(--success)'; }
          } else {
            if (tgDot) tgDot.className = 'tg-dot';
            if (tgFoot) { tgFoot.innerText = data.tg.enabled ? 'Telegram: Connecting' : 'Telegram: Off'; tgFoot.style.color = 'var(--tg-color)'; tgFoot.style.borderColor = 'var(--tg-color)'; }
            if (tgModalStatus) { tgModalStatus.innerText = data.tg.enabled ? '🟡 Wi-Fi Station Connecting...' : '⚪ Telegram Integration Disabled'; tgModalStatus.style.color = 'var(--warning)'; }
          }

          if (!isModalOpen) {
            const tgEn = document.getElementById('tgEnableSwitch');
            if (tgEn) tgEn.checked = data.tg.enabled;
            const tgCid = document.getElementById('tgChatIdInput');
            if (tgCid && !tgCid.value) tgCid.value = data.tg.chat_id || '';
            const tgSsid = document.getElementById('tgStaSsidInput');
            if (tgSsid && !tgSsid.value) tgSsid.value = data.tg.sta_ssid || '';
            const nT = document.getElementById('tgNotifyToggle');
            if (nT) nT.checked = data.tg.n_tog;
            const nTi = document.getElementById('tgNotifyTimer');
            if (nTi) nTi.checked = data.tg.n_tim;
            const nS = document.getElementById('tgNotifySchedule');
            if (nS) nS.checked = data.tg.n_sch;
          }
        }

        // Port 500 status
        if (data.port500 !== undefined) {
          const p500Switch = document.getElementById('port500EnSwitch');
          if (!isModalOpen && p500Switch) p500Switch.checked = data.port500;
          const p500Link = document.getElementById('port500Link');
          if (p500Link) p500Link.style.display = data.port500 ? 'inline' : 'none';
        }

        // Event Logs Stream
        if (data.logs && data.logs.length > 0) {
          const logBox = document.getElementById('eventLogContainer');
          let html = '';
          data.logs.forEach(l => {
            html += `<div class="event-entry"><span class="event-time">[${l.time}]</span><span>${l.msg}</span></div>`;
          });
          logBox.innerHTML = html;
        }

        if (!data.time.synced && !window.hasAutoSynced) {
          window.hasAutoSynced = true;
          syncPhoneTime();
        }
      } catch (err) {
        console.error('Polling error:', err);
      }
    }

    // Port 500 Toggle
    async function togglePort500(en) {
      try {
        const res = await fetch('/api/ota/port500', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: `enable=${en ? 1 : 0}`
        });
        if (res.ok) {
          showToast(`Port 500 OTA Server ${en ? 'Enabled' : 'Disabled'}`);
          fetchStatus();
        }
      } catch(err) {
        showToast('Failed to toggle Port 500');
      }
    }

    // Port 80 OTA
    let port80File = null;
    function onPort80FileSelected() {
      const input = document.getElementById('port80FileInput');
      const info = document.getElementById('port80FileInfo');
      const btn = document.getElementById('port80UploadBtn');
      if (input.files.length > 0) {
        port80File = input.files[0];
        if (!port80File.name.endsWith('.bin')) {
          showToast('Select a compiled .bin file');
          input.value = '';
          port80File = null;
          info.style.display = 'none';
          btn.disabled = true;
          return;
        }
        info.innerText = `Selected: ${port80File.name} (${(port80File.size / 1024).toFixed(1)} KB)`;
        info.style.display = 'block';
        btn.disabled = false;
      } else {
        port80File = null;
        info.style.display = 'none';
        btn.disabled = true;
      }
    }

    function uploadFirmwarePort80() {
      if (!port80File) return;
      const btn = document.getElementById('port80UploadBtn');
      btn.disabled = true;
      showToast('Uploading firmware... Blue LED will blink 5 times on success.');

      const xhr = new XMLHttpRequest();
      const formData = new FormData();
      formData.append('update', port80File, port80File.name);

      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4) {
          if (xhr.status === 200) {
            showToast('🎉 OTA Success! Blue LED blinking 5 times... Rebooting.');
            setTimeout(() => { location.reload(); }, 4500);
          } else {
            showToast('❌ OTA Failed! Blue LED blinking 3 times.');
            btn.disabled = false;
          }
        }
      };
      xhr.open('POST', '/api/ota/upload', true);
      xhr.send(formData);
    }

    setInterval(fetchStatus, 1000);
    fetchStatus();
  </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGES_H
