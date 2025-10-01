import { app, BrowserWindow, ipcMain, dialog } from 'electron';
import { join } from 'path';
import { fileURLToPath } from 'url';

const __dirname = fileURLToPath(new URL('.', import.meta.url));

// The built directory structure
//
// ├─┬─┬ dist
// │ │ └── index.html
// │ │
// │ ├─┬ dist-electron
// │ │ ├── main.js
// │ │ └── preload.mjs
// │
process.env.APP_ROOT = join(__dirname, '..');

export const VITE_DEV_SERVER_URL = process.env['VITE_DEV_SERVER_URL'];
export const MAIN_DIST = join(process.env.APP_ROOT, 'dist-electron');
export const RENDERER_DIST = join(process.env.APP_ROOT, 'dist');

process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL ? join(process.env.APP_ROOT, 'public') : RENDERER_DIST;

let win: BrowserWindow | null;

function createWindow() {
  win = new BrowserWindow({
    width: 1200,
    height: 800,
    minWidth: 800,
    minHeight: 600,
    icon: join(process.env.VITE_PUBLIC, 'electron-vite.svg'),
    webPreferences: {
      preload: join(__dirname, 'preload.mjs'),
      nodeIntegration: false,
      contextIsolation: true,
    },
    titleBarStyle: 'default',
    show: false,
  });

  // Test active push message to Renderer-process.
  win.webContents.on('did-finish-load', () => {
    win?.webContents.send('main-process-message', (new Date).toLocaleString());
  });

  if (VITE_DEV_SERVER_URL) {
    win.loadURL(VITE_DEV_SERVER_URL);
  } else {
    // win.loadFile('dist/index.html')
    win.loadFile(join(RENDERER_DIST, 'index.html'));
  }

  // Show window when ready
  win.once('ready-to-show', () => {
    win?.show();
  });

  // Handle window closed
  win.on('closed', () => {
    win = null;
  });
}

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
    win = null;
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

app.whenReady().then(createWindow);

// IPC handlers for communication with renderer process
ipcMain.handle('app:getVersion', () => {
  return app.getVersion();
});

ipcMain.handle('app:getPath', (event, name) => {
  return app.getPath(name);
});

ipcMain.handle('dialog:showOpenDialog', async (event, options) => {
  const result = await dialog.showOpenDialog(win!, options);
  return result;
});

ipcMain.handle('dialog:showSaveDialog', async (event, options) => {
  const result = await dialog.showSaveDialog(win!, options);
  return result;
});

ipcMain.handle('dialog:showMessageBox', async (event, options) => {
  const result = await dialog.showMessageBox(win!, options);
  return result;
});

// Handle app events
app.on('before-quit', (event) => {
  // Prevent default quit behavior
  event.preventDefault();
  
  // Hide window instead of quitting
  if (win) {
    win.hide();
  }
});

// Handle second instance
const gotTheLock = app.requestSingleInstanceLock();

if (!gotTheLock) {
  app.quit();
} else {
  app.on('second-instance', () => {
    // Someone tried to run a second instance, we should focus our window instead.
    if (win) {
      if (win.isMinimized()) win.restore();
      win.focus();
    }
  });
}