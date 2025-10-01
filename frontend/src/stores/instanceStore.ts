import { create } from 'zustand';
import { InstanceInfo, LaunchStatus } from '@/types';
import { ipcService } from '@/services/IPCService';

interface InstanceStore {
  // State
  instances: InstanceInfo[];
  loading: boolean;
  error: string | null;
  
  // Launch statuses
  launchStatuses: Record<string, LaunchStatus>;
  
  // Actions
  fetchInstances: () => Promise<void>;
  getInstance: (id: string) => Promise<InstanceInfo | null>;
  createInstance: (name: string, version?: string) => Promise<string | null>;
  deleteInstance: (id: string) => Promise<boolean>;
  launchInstance: (id: string) => Promise<boolean>;
  stopInstance: (id: string) => Promise<boolean>;
  getLaunchStatus: (id: string) => Promise<LaunchStatus | null>;
  
  // State setters
  setInstances: (instances: InstanceInfo[]) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
  setLaunchStatus: (id: string, status: LaunchStatus) => void;
  
  // Event handlers
  onInstanceCreated: (instance: InstanceInfo) => void;
  onInstanceDeleted: (id: string) => void;
  onInstanceUpdated: (instance: InstanceInfo) => void;
  onInstanceLaunched: (id: string) => void;
  onInstanceStopped: (id: string) => void;
}

export const useInstanceStore = create<InstanceStore>((set, get) => ({
  // Initial state
  instances: [],
  loading: false,
  error: null,
  launchStatuses: {},
  
  // Fetch instances
  fetchInstances: async () => {
    set({ loading: true, error: null });
    
    try {
      const instances = await ipcService.getInstances();
      set({ instances, loading: false });
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to fetch instances' 
      });
    }
  },
  
  // Get single instance
  getInstance: async (id: string) => {
    try {
      const instance = await ipcService.getInstance(id);
      return instance;
    } catch (error) {
      set({ error: error instanceof Error ? error.message : 'Failed to get instance' });
      return null;
    }
  },
  
  // Create instance
  createInstance: async (name: string, version = 'latest') => {
    set({ loading: true, error: null });
    
    try {
      const id = await ipcService.createInstance(name, version);
      if (id) {
        // Refresh instances list
        await get().fetchInstances();
      }
      set({ loading: false });
      return id;
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to create instance' 
      });
      return null;
    }
  },
  
  // Delete instance
  deleteInstance: async (id: string) => {
    set({ loading: true, error: null });
    
    try {
      const success = await ipcService.deleteInstance(id);
      if (success) {
        // Remove from local state
        set(state => ({
          instances: state.instances.filter(instance => instance.id !== id),
          loading: false,
        }));
      } else {
        set({ loading: false, error: 'Failed to delete instance' });
      }
      return success;
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to delete instance' 
      });
      return false;
    }
  },
  
  // Launch instance
  launchInstance: async (id: string) => {
    set({ loading: true, error: null });
    
    try {
      const success = await ipcService.launchInstance(id);
      if (success) {
        // Update instance status
        set(state => ({
          instances: state.instances.map(instance =>
            instance.id === id ? { ...instance, isRunning: true } : instance
          ),
          loading: false,
        }));
        
        // Get launch status
        const status = await ipcService.getLaunchStatus(id);
        if (status) {
          set(state => ({
            launchStatuses: { ...state.launchStatuses, [id]: status },
          }));
        }
      } else {
        set({ loading: false, error: 'Failed to launch instance' });
      }
      return success;
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to launch instance' 
      });
      return false;
    }
  },
  
  // Stop instance
  stopInstance: async (id: string) => {
    set({ loading: true, error: null });
    
    try {
      const success = await ipcService.stopInstance(id);
      if (success) {
        // Update instance status
        set(state => ({
          instances: state.instances.map(instance =>
            instance.id === id ? { ...instance, isRunning: false } : instance
          ),
          loading: false,
        }));
        
        // Update launch status
        set(state => ({
          launchStatuses: {
            ...state.launchStatuses,
            [id]: { ...state.launchStatuses[id], isRunning: false, status: 'stopped' },
          },
        }));
      } else {
        set({ loading: false, error: 'Failed to stop instance' });
      }
      return success;
    } catch (error) {
      set({ 
        loading: false, 
        error: error instanceof Error ? error.message : 'Failed to stop instance' 
      });
      return false;
    }
  },
  
  // Get launch status
  getLaunchStatus: async (id: string) => {
    try {
      const status = await ipcService.getLaunchStatus(id);
      if (status) {
        set(state => ({
          launchStatuses: { ...state.launchStatuses, [id]: status },
        }));
      }
      return status;
    } catch (error) {
      set({ error: error instanceof Error ? error.message : 'Failed to get launch status' });
      return null;
    }
  },
  
  // State setters
  setInstances: (instances) => set({ instances }),
  setLoading: (loading) => set({ loading }),
  setError: (error) => set({ error }),
  setLaunchStatus: (id, status) => set(state => ({
    launchStatuses: { ...state.launchStatuses, [id]: status },
  })),
  
  // Event handlers
  onInstanceCreated: (instance) => {
    set(state => ({
      instances: [...state.instances, instance],
    }));
  },
  
  onInstanceDeleted: (id) => {
    set(state => ({
      instances: state.instances.filter(instance => instance.id !== id),
      launchStatuses: Object.fromEntries(
        Object.entries(state.launchStatuses).filter(([key]) => key !== id)
      ),
    }));
  },
  
  onInstanceUpdated: (instance) => {
    set(state => ({
      instances: state.instances.map(i => i.id === instance.id ? instance : i),
    }));
  },
  
  onInstanceLaunched: (id) => {
    set(state => ({
      instances: state.instances.map(instance =>
        instance.id === id ? { ...instance, isRunning: true } : instance
      ),
    }));
  },
  
  onInstanceStopped: (id) => {
    set(state => ({
      instances: state.instances.map(instance =>
        instance.id === id ? { ...instance, isRunning: false } : instance
      ),
    }));
  },
}));