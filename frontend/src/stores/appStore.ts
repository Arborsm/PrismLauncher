import { create } from 'zustand';
import { SystemInfo, AppSettings } from '@/types';
import { ipcService } from '@/services/IPCService';

interface AppStore {
  // Theme
  theme: 'light' | 'dark' | 'auto';
  setTheme: (theme: 'light' | 'dark' | 'auto') => void;
  
  // System info
  systemInfo: SystemInfo | null;
  setSystemInfo: (systemInfo: SystemInfo) => void;
  
  // Settings
  settings: AppSettings | null;
  setSettings: (settings: AppSettings) => void;
  
  // Loading states
  loading: boolean;
  setLoading: (loading: boolean) => void;
  
  // Error handling
  error: string | null;
  setError: (error: string | null) => void;
  
  // Actions
  initialize: () => Promise<void>;
  loadSystemInfo: () => Promise<void>;
  loadSettings: () => Promise<void>;
  saveSettings: (settings: Partial<AppSettings>) => Promise<void>;
}

export const useAppStore = create<AppStore>((set, get) => ({
  // Initial state
  theme: 'dark',
  systemInfo: null,
  settings: null,
  loading: false,
  error: null,
  
  // Theme actions
  setTheme: (theme) => {
    set({ theme });
    // Save theme preference
    const { settings } = get();
    if (settings) {
      get().saveSettings({ theme });
    }
  },
  
  // System info actions
  setSystemInfo: (systemInfo) => set({ systemInfo }),
  
  // Settings actions
  setSettings: (settings) => set({ settings }),
  
  // Loading actions
  setLoading: (loading) => set({ loading }),
  
  // Error actions
  setError: (error) => set({ error }),
  
  // Initialize app
  initialize: async () => {
    set({ loading: true, error: null });
    
    try {
      // Connect to IPC service
      await ipcService.connect();
      
      // Load system info and settings
      await Promise.all([
        get().loadSystemInfo(),
        get().loadSettings(),
      ]);
      
      set({ loading: false });
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to initialize app' 
      });
    }
  },
  
  // Load system info
  loadSystemInfo: async () => {
    try {
      const systemInfo = await ipcService.getSystemInfo();
      set({ systemInfo });
    } catch (error) {
      set({ error: error instanceof Error ? error.message : 'Failed to load system info' });
    }
  },
  
  // Load settings
  loadSettings: async () => {
    try {
      const settings = await ipcService.getSettings();
      set({ settings, theme: settings.theme });
    } catch (error) {
      set({ error: error instanceof Error ? error.message : 'Failed to load settings' });
    }
  },
  
  // Save settings
  saveSettings: async (newSettings) => {
    try {
      const { settings } = get();
      if (settings) {
        const updatedSettings = { ...settings, ...newSettings };
        await ipcService.setSettings(updatedSettings);
        set({ settings: updatedSettings });
      }
    } catch (error) {
      set({ error: error instanceof Error ? error.message : 'Failed to save settings' });
    }
  },
}));