use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use sysinfo::{System, SystemExt};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SystemInfo {
    pub os_name: String,
    pub os_version: String,
    pub architecture: String,
    pub java_version: String,
    pub launcher_version: String,
    pub properties: HashMap<String, String>,
}

impl SystemInfo {
    pub fn new() -> Self {
        let mut system = System::new_all();
        system.refresh_all();
        
        let mut properties = HashMap::new();
        properties.insert("total_memory".to_string(), format!("{} MB", system.total_memory()));
        properties.insert("used_memory".to_string(), format!("{} MB", system.used_memory()));
        properties.insert("cpu_count".to_string(), system.cpus().len().to_string());
        
        Self {
            os_name: std::env::consts::OS.to_string(),
            os_version: get_os_version(),
            architecture: std::env::consts::ARCH.to_string(),
            java_version: "17.0.2".to_string(), // 实际实现中会检测Java版本
            launcher_version: env!("CARGO_PKG_VERSION").to_string(),
            properties,
        }
    }
}

fn get_os_version() -> String {
    #[cfg(target_os = "windows")]
    {
        // Windows版本检测
        "Windows 10/11".to_string()
    }
    
    #[cfg(target_os = "macos")]
    {
        // macOS版本检测
        "macOS".to_string()
    }
    
    #[cfg(target_os = "linux")]
    {
        // Linux版本检测
        "Linux".to_string()
    }
    
    #[cfg(not(any(target_os = "windows", target_os = "macos", target_os = "linux")))]
    {
        "Unknown".to_string()
    }
}

pub struct SystemManager {
    system_info: SystemInfo,
}

impl SystemManager {
    pub fn new() -> Self {
        Self {
            system_info: SystemInfo::new(),
        }
    }
    
    pub async fn get_system_info(&self) -> Result<SystemInfo, String> {
        Ok(self.system_info.clone())
    }
    
    pub async fn refresh_system_info(&mut self) -> Result<SystemInfo, String> {
        self.system_info = SystemInfo::new();
        Ok(self.system_info.clone())
    }
    
    pub async fn get_launcher_info(&self) -> Result<HashMap<String, String>, String> {
        let mut info = HashMap::new();
        info.insert("name".to_string(), "Prism Launcher".to_string());
        info.insert("version".to_string(), env!("CARGO_PKG_VERSION").to_string());
        info.insert("author".to_string(), "PrismLauncher Team".to_string());
        info.insert("description".to_string(), "Modern Minecraft launcher".to_string());
        Ok(info)
    }
}