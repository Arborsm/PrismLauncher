use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;
use chrono::{DateTime, Utc};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InstanceInfo {
    pub id: String,
    pub name: String,
    pub version: String,
    pub icon_path: String,
    pub instance_path: String,
    pub is_running: bool,
    pub metadata: HashMap<String, String>,
    pub created_time: DateTime<Utc>,
    pub last_played_time: DateTime<Utc>,
}

impl InstanceInfo {
    pub fn new(name: String, version: String, instance_path: String) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4().to_string(),
            name,
            version,
            icon_path: String::new(),
            instance_path,
            is_running: false,
            metadata: HashMap::new(),
            created_time: now,
            last_played_time: now,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LaunchStatus {
    pub instance_id: String,
    pub is_running: bool,
    pub process_id: Option<u32>,
    pub status: String, // "starting", "running", "stopped", "error"
    pub error_message: String,
}

pub struct InstanceManager {
    instances: Vec<InstanceInfo>,
    launch_statuses: HashMap<String, LaunchStatus>,
}

impl InstanceManager {
    pub fn new() -> Self {
        Self {
            instances: Vec::new(),
            launch_statuses: HashMap::new(),
        }
    }
    
    pub async fn get_instances(&self) -> Result<Vec<InstanceInfo>, String> {
        Ok(self.instances.clone())
    }
    
    pub async fn get_instance(&self, id: &str) -> Result<Option<InstanceInfo>, String> {
        Ok(self.instances.iter().find(|i| i.id == id).cloned())
    }
    
    pub async fn create_instance(&mut self, name: String, version: String) -> Result<String, String> {
        let instance_path = format!("./instances/{}", name);
        let instance = InstanceInfo::new(name, version, instance_path);
        let instance_id = instance.id.clone();
        
        self.instances.push(instance);
        Ok(instance_id)
    }
    
    pub async fn delete_instance(&mut self, id: String) -> Result<bool, String> {
        let initial_len = self.instances.len();
        self.instances.retain(|i| i.id != id);
        
        // 同时清理启动状态
        self.launch_statuses.remove(&id);
        
        Ok(self.instances.len() < initial_len)
    }
    
    pub async fn copy_instance(&mut self, id: String, new_name: String) -> Result<String, String> {
        if let Some(original) = self.instances.iter().find(|i| i.id == id) {
            let new_instance_path = format!("./instances/{}", new_name);
            let mut new_instance = InstanceInfo::new(new_name, original.version.clone(), new_instance_path);
            new_instance.metadata = original.metadata.clone();
            
            let new_id = new_instance.id.clone();
            self.instances.push(new_instance);
            Ok(new_id)
        } else {
            Err("Instance not found".to_string())
        }
    }
    
    pub async fn launch_instance(&mut self, id: String) -> Result<bool, String> {
        if let Some(instance) = self.instances.iter_mut().find(|i| i.id == id) {
            if instance.is_running {
                return Err("Instance is already running".to_string());
            }
            
            instance.is_running = true;
            instance.last_played_time = Utc::now();
            
            // 创建启动状态
            let launch_status = LaunchStatus {
                instance_id: id.clone(),
                is_running: true,
                process_id: Some(std::process::id()),
                status: "running".to_string(),
                error_message: String::new(),
            };
            
            self.launch_statuses.insert(id, launch_status);
            Ok(true)
        } else {
            Err("Instance not found".to_string())
        }
    }
    
    pub async fn stop_instance(&mut self, id: String) -> Result<bool, String> {
        if let Some(instance) = self.instances.iter_mut().find(|i| i.id == id) {
            if !instance.is_running {
                return Err("Instance is not running".to_string());
            }
            
            instance.is_running = false;
            
            // 更新启动状态
            if let Some(status) = self.launch_statuses.get_mut(&id) {
                status.is_running = false;
                status.status = "stopped".to_string();
            }
            
            Ok(true)
        } else {
            Err("Instance not found".to_string())
        }
    }
    
    pub async fn get_launch_status(&self, id: &str) -> Result<Option<LaunchStatus>, String> {
        Ok(self.launch_statuses.get(id).cloned())
    }
    
    pub async fn update_instance(&mut self, id: String, updates: HashMap<String, String>) -> Result<bool, String> {
        if let Some(instance) = self.instances.iter_mut().find(|i| i.id == id) {
            if let Some(name) = updates.get("name") {
                instance.name = name.clone();
            }
            if let Some(version) = updates.get("version") {
                instance.version = version.clone();
            }
            if let Some(icon_path) = updates.get("icon_path") {
                instance.icon_path = icon_path.clone();
            }
            
            Ok(true)
        } else {
            Err("Instance not found".to_string())
        }
    }
}