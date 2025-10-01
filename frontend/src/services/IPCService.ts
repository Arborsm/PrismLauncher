import { create } from 'zustand';
import { InstanceInfo, AccountInfo, JavaInfo, SystemInfo, ModInfo, LaunchStatus, AppSettings } from '@/types';
import { tauriService } from './TauriService';

// IPC Service interface
export interface IPCService {
  // Connection
  connect(): Promise<boolean>;
  disconnect(): void;
  isConnected(): boolean;

  // Instance management
  getInstances(): Promise<InstanceInfo[]>;
  getInstance(id: string): Promise<InstanceInfo>;
  createInstance(name: string, version?: string): Promise<string>;
  deleteInstance(id: string): Promise<boolean>;
  launchInstance(id: string): Promise<boolean>;
  stopInstance(id: string): Promise<boolean>;
  getLaunchStatus(id: string): Promise<LaunchStatus>;

  // Account management
  getAccounts(): Promise<AccountInfo[]>;
  addAccount(username: string, type: 'microsoft' | 'offline'): Promise<string>;
  removeAccount(id: string): Promise<boolean>;
  loginAccount(id: string): Promise<boolean>;
  logoutAccount(id: string): Promise<boolean>;

  // Java management
  getJavaInstalls(): Promise<JavaInfo[]>;
  getJavaInfo(id: string): Promise<JavaInfo>;
  installJava(version: string): Promise<boolean>;
  uninstallJava(id: string): Promise<boolean>;

  // Settings
  getSettings(): Promise<AppSettings>;
  setSettings(settings: Partial<AppSettings>): Promise<boolean>;

  // Mod platform
  searchMods(query: string, platform?: string): Promise<ModInfo[]>;
  getModInfo(id: string): Promise<ModInfo>;
  installMod(id: string): Promise<boolean>;
  uninstallMod(id: string): Promise<boolean>;

  // System info
  getSystemInfo(): Promise<SystemInfo>;

  // Events
  onEvent(callback: (event: any) => void): void;
  offEvent(callback: (event: any) => void): void;
}

// Tauri IPC Service implementation
class TauriIPCService implements IPCService {
  private connected = true; // Tauri is always connected
  private eventCallbacks: ((event: any) => void)[] = [];

  async connect(): Promise<boolean> {
    this.connected = true;
    return true;
  }

  disconnect(): void {
    this.connected = false;
  }

  isConnected(): boolean {
    return this.connected;
  }

  async getInstances(): Promise<InstanceInfo[]> {
    return await tauriService.getInstances();
  }

  async getInstance(id: string): Promise<InstanceInfo> {
    const instances = await this.getInstances();
    const instance = instances.find(i => i.id === id);
    if (!instance) {
      throw new Error(`Instance with id ${id} not found`);
    }
    return instance;
  }

  async createInstance(name: string, version = 'latest'): Promise<string> {
    const id = await tauriService.createInstance(name, version);
    
    // Emit event
    this.emitEvent({
      type: 'instance.created',
      data: { instanceId: id, instance: { id, name, version } },
      timestamp: new Date(),
    });

    return id;
  }

  async deleteInstance(id: string): Promise<boolean> {
    const result = await tauriService.deleteInstance(id);
    
    if (result) {
      // Emit event
      this.emitEvent({
        type: 'instance.deleted',
        data: { instanceId: id },
        timestamp: new Date(),
      });
    }

    return result;
  }

  async launchInstance(id: string): Promise<boolean> {
    const result = await tauriService.launchInstance(id);
    
    if (result) {
      // Emit event
      this.emitEvent({
        type: 'instance.launched',
        data: { instanceId: id },
        timestamp: new Date(),
      });
    }

    return result;
  }

  async stopInstance(id: string): Promise<boolean> {
    const result = await tauriService.stopInstance(id);
    
    if (result) {
      // Emit event
      this.emitEvent({
        type: 'instance.stopped',
        data: { instanceId: id },
        timestamp: new Date(),
      });
    }

    return result;
  }

  async getLaunchStatus(id: string): Promise<LaunchStatus> {
    // Tauri implementation would get real launch status
    return {
      instanceId: id,
      isRunning: false,
      processId: 0,
      status: 'stopped',
      errorMessage: '',
    };
  }

  async getAccounts(): Promise<AccountInfo[]> {
    return await tauriService.getAccounts();
  }

  async addAccount(username: string, type: 'microsoft' | 'offline'): Promise<string> {
    const id = await tauriService.addAccount(username, type);
    
    // Emit event
    this.emitEvent({
      type: 'account.added',
      data: { accountId: id, account: { id, username, type } },
      timestamp: new Date(),
    });

    return id;
  }

  async removeAccount(id: string): Promise<boolean> {
    const result = await tauriService.removeAccount(id);
    
    if (result) {
      // Emit event
      this.emitEvent({
        type: 'account.removed',
        data: { accountId: id },
        timestamp: new Date(),
      });
    }

    return result;
  }

  async loginAccount(id: string): Promise<boolean> {
    // Tauri implementation would handle login
    this.emitEvent({
      type: 'account.logged_in',
      data: { accountId: id },
      timestamp: new Date(),
    });
    return true;
  }

  async logoutAccount(id: string): Promise<boolean> {
    // Tauri implementation would handle logout
    this.emitEvent({
      type: 'account.logged_out',
      data: { accountId: id },
      timestamp: new Date(),
    });
    return true;
  }

  async getJavaInstalls(): Promise<JavaInfo[]> {
    return await tauriService.getJavaInstalls();
  }

  async getJavaInfo(id: string): Promise<JavaInfo> {
    const javaInstalls = await this.getJavaInstalls();
    const java = javaInstalls.find(j => j.id === id);
    if (!java) {
      throw new Error(`Java install with id ${id} not found`);
    }
    return java;
  }

  async installJava(version: string): Promise<boolean> {
    return await tauriService.installJava(version);
  }

  async uninstallJava(id: string): Promise<boolean> {
    // Tauri implementation would handle uninstall
    return true;
  }

  async getSettings(): Promise<AppSettings> {
    return await tauriService.getSettings();
  }

  async setSettings(settings: Partial<AppSettings>): Promise<boolean> {
    const currentSettings = await this.getSettings();
    const newSettings = { ...currentSettings, ...settings };
    return await tauriService.setSettings(newSettings);
  }

  async searchMods(query: string, platform = 'curseforge'): Promise<ModInfo[]> {
    // Tauri implementation would search mods
    return [
      {
        id: 'mod1',
        name: 'Test Mod',
        version: '1.0.0',
        description: 'A test mod for demonstration',
        author: 'TestAuthor',
        platform: platform as any,
        downloadUrl: 'https://example.com/mod.jar',
        filePath: './mods/mod.jar',
      },
    ];
  }

  async getModInfo(id: string): Promise<ModInfo> {
    const mods = await this.searchMods('');
    const mod = mods.find(m => m.id === id);
    if (!mod) {
      throw new Error(`Mod with id ${id} not found`);
    }
    return mod;
  }

  async installMod(id: string): Promise<boolean> {
    // Tauri implementation would install mod
    return true;
  }

  async uninstallMod(id: string): Promise<boolean> {
    // Tauri implementation would uninstall mod
    return true;
  }

  async getSystemInfo(): Promise<SystemInfo> {
    return await tauriService.getSystemInfo();
  }

  onEvent(callback: (event: any) => void): void {
    this.eventCallbacks.push(callback);
  }

  offEvent(callback: (event: any) => void): void {
    const index = this.eventCallbacks.indexOf(callback);
    if (index > -1) {
      this.eventCallbacks.splice(index, 1);
    }
  }

  private emitEvent(event: any): void {
    this.eventCallbacks.forEach(callback => {
      try {
        callback(event);
      } catch (error) {
        console.error('Error in event callback:', error);
      }
    });
  }
}

// Create singleton instance
export const ipcService = new TauriIPCService();

// Store for managing IPC service state
interface IPCStore {
  connected: boolean;
  connecting: boolean;
  error: string | null;
  setConnected: (connected: boolean) => void;
  setConnecting: (connecting: boolean) => void;
  setError: (error: string | null) => void;
}

export const useIPCStore = create<IPCStore>((set) => ({
  connected: false,
  connecting: false,
  error: null,
  setConnected: (connected) => set({ connected }),
  setConnecting: (connecting) => set({ connecting }),
  setError: (error) => set({ error }),
}));