use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ModInfo {
    pub id: String,
    pub name: String,
    pub version: String,
    pub description: String,
    pub author: String,
    pub platform: String, // "curseforge", "modrinth", "local"
    pub download_url: String,
    pub file_path: String,
    pub metadata: HashMap<String, String>,
}

impl ModInfo {
    pub fn new(
        name: String,
        version: String,
        description: String,
        author: String,
        platform: String,
    ) -> Self {
        Self {
            id: Uuid::new_v4().to_string(),
            name,
            version,
            description,
            author,
            platform,
            download_url: String::new(),
            file_path: String::new(),
            metadata: HashMap::new(),
        }
    }
}

pub struct ModPlatformManager {
    mods: Vec<ModInfo>,
}

impl ModPlatformManager {
    pub fn new() -> Self {
        Self {
            mods: Vec::new(),
        }
    }
    
    pub async fn search_mods(&self, query: &str, platform: &str) -> Result<Vec<ModInfo>, String> {
        // 模拟搜索模组
        let mut results = Vec::new();
        
        if query.contains("test") {
            let mod_info = ModInfo::new(
                "Test Mod".to_string(),
                "1.0.0".to_string(),
                "A test mod for demonstration".to_string(),
                "TestAuthor".to_string(),
                platform.to_string(),
            );
            results.push(mod_info);
        }
        
        Ok(results)
    }
    
    pub async fn get_mod_info(&self, id: &str) -> Result<Option<ModInfo>, String> {
        Ok(self.mods.iter().find(|m| m.id == id).cloned())
    }
    
    pub async fn install_mod(&mut self, id: String) -> Result<bool, String> {
        // 模拟安装模组
        if let Some(mod_info) = self.mods.iter().find(|m| m.id == id) {
            // 实际实现中会下载和安装模组
            Ok(true)
        } else {
            Err("Mod not found".to_string())
        }
    }
    
    pub async fn uninstall_mod(&mut self, id: String) -> Result<bool, String> {
        let initial_len = self.mods.len();
        self.mods.retain(|m| m.id != id);
        Ok(self.mods.len() < initial_len)
    }
    
    pub async fn update_mod(&mut self, id: String) -> Result<bool, String> {
        // 模拟更新模组
        if let Some(mod_info) = self.mods.iter_mut().find(|m| m.id == id) {
            // 实际实现中会检查更新并下载新版本
            Ok(true)
        } else {
            Err("Mod not found".to_string())
        }
    }
    
    pub async fn get_installed_mods(&self) -> Result<Vec<ModInfo>, String> {
        Ok(self.mods.clone())
    }
    
    pub async fn add_local_mod(&mut self, file_path: String) -> Result<String, String> {
        // 添加本地模组文件
        let mod_info = ModInfo::new(
            "Local Mod".to_string(),
            "1.0.0".to_string(),
            "Local mod file".to_string(),
            "Unknown".to_string(),
            "local".to_string(),
        );
        
        let mod_id = mod_info.id.clone();
        self.mods.push(mod_info);
        Ok(mod_id)
    }
}