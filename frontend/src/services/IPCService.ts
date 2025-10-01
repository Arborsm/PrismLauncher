import { create } from 'zustand';
import { InstanceInfo, AccountInfo, JavaInfo, SystemInfo, ModInfo, LaunchStatus, AppSettings } from '@/types';

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

// Mock IPC Service implementation
class MockIPCService implements IPCService {
  private connected = false;
  private eventCallbacks: ((event: any) => void)[] = [];

  async connect(): Promise<boolean> {
    // Simulate connection delay
    await new Promise(resolve => setTimeout(resolve, 1000));
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
    await this.simulateDelay();
    return [
      {
        id: 'instance1',
        name: 'Test Instance 1',
        version: '1.20.1',
        iconPath: '',
        instancePath: './instances/test1',
        isRunning: false,
      },
      {
        id: 'instance2',
        name: 'Test Instance 2',
        version: '1.19.4',
        iconPath: '',
        instancePath: './instances/test2',
        isRunning: true,
      },
    ];
  }

  async getInstance(id: string): Promise<InstanceInfo> {
    await this.simulateDelay();
    const instances = await this.getInstances();
    const instance = instances.find(i => i.id === id);
    if (!instance) {
      throw new Error(`Instance with id ${id} not found`);
    }
    return instance;
  }

  async createInstance(name: string, version = 'latest'): Promise<string> {
    await this.simulateDelay();
    const id = `instance_${Date.now()}`;
    
    // Emit event
    this.emitEvent({
      type: 'instance.created',
      data: { instanceId: id, instance: { id, name, version } },
      timestamp: new Date(),
    });

    return id;
  }

  async deleteInstance(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'instance.deleted',
      data: { instanceId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async launchInstance(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'instance.launched',
      data: { instanceId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async stopInstance(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'instance.stopped',
      data: { instanceId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async getLaunchStatus(id: string): Promise<LaunchStatus> {
    await this.simulateDelay();
    return {
      instanceId: id,
      isRunning: Math.random() > 0.5,
      processId: Math.floor(Math.random() * 10000) + 1000,
      status: 'running',
      errorMessage: '',
    };
  }

  async getAccounts(): Promise<AccountInfo[]> {
    await this.simulateDelay();
    return [
      {
        id: 'account1',
        username: 'TestUser',
        uuid: '12345678-1234-1234-1234-123456789012',
        type: 'offline',
        isValid: true,
      },
    ];
  }

  async addAccount(username: string, type: 'microsoft' | 'offline'): Promise<string> {
    await this.simulateDelay();
    const id = `account_${Date.now()}`;
    
    // Emit event
    this.emitEvent({
      type: 'account.added',
      data: { accountId: id, account: { id, username, type } },
      timestamp: new Date(),
    });

    return id;
  }

  async removeAccount(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'account.removed',
      data: { accountId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async loginAccount(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'account.logged_in',
      data: { accountId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async logoutAccount(id: string): Promise<boolean> {
    await this.simulateDelay();
    
    // Emit event
    this.emitEvent({
      type: 'account.logged_out',
      data: { accountId: id },
      timestamp: new Date(),
    });

    return true;
  }

  async getJavaInstalls(): Promise<JavaInfo[]> {
    await this.simulateDelay();
    return [
      {
        id: 'java1',
        version: '17.0.2',
        path: '/usr/lib/jvm/java-17-openjdk',
        architecture: 'x86_64',
        isValid: true,
      },
    ];
  }

  async getJavaInfo(id: string): Promise<JavaInfo> {
    await this.simulateDelay();
    const javaInstalls = await this.getJavaInstalls();
    const java = javaInstalls.find(j => j.id === id);
    if (!java) {
      throw new Error(`Java install with id ${id} not found`);
    }
    return java;
  }

  async installJava(version: string): Promise<boolean> {
    await this.simulateDelay();
    return true;
  }

  async uninstallJava(id: string): Promise<boolean> {
    await this.simulateDelay();
    return true;
  }

  async getSettings(): Promise<AppSettings> {
    await this.simulateDelay();
    return {
      theme: 'dark',
      language: 'en',
      autoUpdate: true,
      minimizeToTray: true,
      startMinimized: false,
      memorySize: 4096,
      windowWidth: 1200,
      windowHeight: 800,
    };
  }

  async setSettings(settings: Partial<AppSettings>): Promise<boolean> {
    await this.simulateDelay();
    return true;
  }

  async searchMods(query: string, platform = 'curseforge'): Promise<ModInfo[]> {
    await this.simulateDelay();
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
    await this.simulateDelay();
    const mods = await this.searchMods('');
    const mod = mods.find(m => m.id === id);
    if (!mod) {
      throw new Error(`Mod with id ${id} not found`);
    }
    return mod;
  }

  async installMod(id: string): Promise<boolean> {
    await this.simulateDelay();
    return true;
  }

  async uninstallMod(id: string): Promise<boolean> {
    await this.simulateDelay();
    return true;
  }

  async getSystemInfo(): Promise<SystemInfo> {
    await this.simulateDelay();
    return {
      osName: 'Linux',
      osVersion: '6.12.8+',
      architecture: 'x86_64',
      javaVersion: '17.0.2',
      launcherVersion: '10.0.0',
    };
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

  private async simulateDelay(): Promise<void> {
    await new Promise(resolve => setTimeout(resolve, Math.random() * 500 + 100));
  }
}

// Create singleton instance
export const ipcService = new MockIPCService();

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