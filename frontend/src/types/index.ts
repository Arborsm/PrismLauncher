// Core data types matching the IPC interface
export interface InstanceInfo {
  id: string;
  name: string;
  version: string;
  iconPath: string;
  instancePath: string;
  isRunning: boolean;
  metadata?: Record<string, string>;
}

export interface AccountInfo {
  id: string;
  username: string;
  uuid: string;
  type: 'microsoft' | 'offline';
  isValid: boolean;
  properties?: Record<string, string>;
}

export interface JavaInfo {
  id: string;
  version: string;
  path: string;
  architecture: string;
  isValid: boolean;
  properties?: Record<string, string>;
}

export interface LaunchStatus {
  instanceId: string;
  isRunning: boolean;
  processId: number;
  status: 'starting' | 'running' | 'stopped' | 'error';
  errorMessage: string;
}

export interface ModInfo {
  id: string;
  name: string;
  version: string;
  description: string;
  author: string;
  platform: 'curseforge' | 'modrinth' | 'local';
  downloadUrl: string;
  filePath: string;
  metadata?: Record<string, string>;
}

export interface SystemInfo {
  osName: string;
  osVersion: string;
  architecture: string;
  javaVersion: string;
  launcherVersion: string;
  properties?: Record<string, string>;
}

// UI specific types
export interface Theme {
  name: string;
  displayName: string;
  colors: {
    primary: string;
    secondary: string;
    background: string;
    surface: string;
    text: string;
    textSecondary: string;
    border: string;
    error: string;
    warning: string;
    success: string;
  };
}

export interface AppSettings {
  theme: 'light' | 'dark' | 'auto';
  language: string;
  autoUpdate: boolean;
  minimizeToTray: boolean;
  startMinimized: boolean;
  javaPath?: string;
  memorySize: number;
  windowWidth: number;
  windowHeight: number;
}

export interface NotificationData {
  id: string;
  type: 'info' | 'success' | 'warning' | 'error';
  title: string;
  message: string;
  timestamp: Date;
  read: boolean;
}

// Component props types
export interface InstanceCardProps {
  instance: InstanceInfo;
  onLaunch: (id: string) => void;
  onStop: (id: string) => void;
  onEdit: (id: string) => void;
  onDelete: (id: string) => void;
}

export interface InstanceListProps {
  instances: InstanceInfo[];
  loading: boolean;
  onRefresh: () => void;
  onCreate: () => void;
}

export interface AccountCardProps {
  account: AccountInfo;
  onLogin: (id: string) => void;
  onLogout: (id: string) => void;
  onRemove: (id: string) => void;
}

export interface JavaCardProps {
  java: JavaInfo;
  onSelect: (id: string) => void;
  onRemove: (id: string) => void;
}

export interface ModCardProps {
  mod: ModInfo;
  onInstall: (id: string) => void;
  onUninstall: (id: string) => void;
  onUpdate: (id: string) => void;
}

// API response types
export interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: string;
  message?: string;
}

export interface PaginatedResponse<T> {
  items: T[];
  total: number;
  page: number;
  pageSize: number;
  hasMore: boolean;
}

// Event types
export interface LauncherEvent {
  type: string;
  data: any;
  timestamp: Date;
}

export interface InstanceEvent extends LauncherEvent {
  type: 'instance.created' | 'instance.deleted' | 'instance.updated' | 'instance.launched' | 'instance.stopped';
  data: {
    instanceId: string;
    instance?: InstanceInfo;
  };
}

export interface AccountEvent extends LauncherEvent {
  type: 'account.added' | 'account.removed' | 'account.logged_in' | 'account.logged_out';
  data: {
    accountId: string;
    account?: AccountInfo;
  };
}

// Navigation types
export type NavigationItem = {
  id: string;
  label: string;
  icon: string;
  path: string;
  badge?: number;
};

export type NavigationSection = {
  id: string;
  label: string;
  items: NavigationItem[];
};

// Form types
export interface CreateInstanceForm {
  name: string;
  version: string;
  type: 'vanilla' | 'forge' | 'fabric' | 'quilt';
  javaVersion?: string;
  memorySize: number;
}

export interface AddAccountForm {
  username: string;
  type: 'microsoft' | 'offline';
  password?: string;
}

export interface SettingsForm {
  theme: 'light' | 'dark' | 'auto';
  language: string;
  autoUpdate: boolean;
  minimizeToTray: boolean;
  startMinimized: boolean;
  javaPath?: string;
  memorySize: number;
}