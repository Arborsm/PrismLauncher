import { invoke } from '@tauri-apps/api/tauri';
import { InstanceInfo, AccountInfo, JavaInfo, SystemInfo, AppSettings } from '@/types';

// Tauri服务接口
export interface TauriService {
  // IPC通信
  handleIPCRequest(request: any): Promise<any>;
  
  // 实例管理
  getInstances(): Promise<InstanceInfo[]>;
  createInstance(name: string, version?: string): Promise<string>;
  deleteInstance(id: string): Promise<boolean>;
  launchInstance(id: string): Promise<boolean>;
  stopInstance(id: string): Promise<boolean>;
  
  // 账户管理
  getAccounts(): Promise<AccountInfo[]>;
  addAccount(username: string, type: 'microsoft' | 'offline'): Promise<string>;
  removeAccount(id: string): Promise<boolean>;
  
  // Java管理
  getJavaInstalls(): Promise<JavaInfo[]>;
  installJava(version: string): Promise<boolean>;
  
  // 系统信息
  getSystemInfo(): Promise<SystemInfo>;
  
  // 设置管理
  getSettings(): Promise<AppSettings>;
  setSettings(settings: AppSettings): Promise<boolean>;
  
  // 文件操作
  openFile(path: string): Promise<void>;
  openFolder(path: string): Promise<void>;
}

// Tauri服务实现
class TauriServiceImpl implements TauriService {
  async handleIPCRequest(request: any): Promise<any> {
    return await invoke('handle_ipc_request', { request });
  }
  
  async getInstances(): Promise<InstanceInfo[]> {
    return await invoke('get_instances');
  }
  
  async createInstance(name: string, version = 'latest'): Promise<string> {
    return await invoke('create_instance', { name, version });
  }
  
  async deleteInstance(id: string): Promise<boolean> {
    return await invoke('delete_instance', { id });
  }
  
  async launchInstance(id: string): Promise<boolean> {
    return await invoke('launch_instance', { id });
  }
  
  async stopInstance(id: string): Promise<boolean> {
    return await invoke('stop_instance', { id });
  }
  
  async getAccounts(): Promise<AccountInfo[]> {
    return await invoke('get_accounts');
  }
  
  async addAccount(username: string, type: 'microsoft' | 'offline'): Promise<string> {
    return await invoke('add_account', { username, accountType: type });
  }
  
  async removeAccount(id: string): Promise<boolean> {
    return await invoke('remove_account', { id });
  }
  
  async getJavaInstalls(): Promise<JavaInfo[]> {
    return await invoke('get_java_installs');
  }
  
  async installJava(version: string): Promise<boolean> {
    return await invoke('install_java', { version });
  }
  
  async getSystemInfo(): Promise<SystemInfo> {
    return await invoke('get_system_info');
  }
  
  async getSettings(): Promise<AppSettings> {
    return await invoke('get_settings');
  }
  
  async setSettings(settings: AppSettings): Promise<boolean> {
    return await invoke('set_settings', { settings });
  }
  
  async openFile(path: string): Promise<void> {
    return await invoke('open_file', { path });
  }
  
  async openFolder(path: string): Promise<void> {
    return await invoke('open_folder', { path });
  }
}

// 创建单例实例
export const tauriService = new TauriServiceImpl();

// 更新现有的IPC服务以使用Tauri
export const ipcService = tauriService;