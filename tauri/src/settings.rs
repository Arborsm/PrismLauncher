use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use std::path::Path;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AppSettings {
    pub theme: String, // "light", "dark", "auto"
    pub language: String,
    pub auto_update: bool,
    pub minimize_to_tray: bool,
    pub start_minimized: bool,
    pub java_path: Option<String>,
    pub memory_size: u32,
    pub window_width: u32,
    pub window_height: u32,
    pub custom_settings: HashMap<String, String>,
}

impl Default for AppSettings {
    fn default() -> Self {
        Self {
            theme: "dark".to_string(),
            language: "en".to_string(),
            auto_update: true,
            minimize_to_tray: true,
            start_minimized: false,
            java_path: None,
            memory_size: 4096,
            window_width: 1200,
            window_height: 800,
            custom_settings: HashMap::new(),
        }
    }
}

pub struct SettingsManager {
    settings: AppSettings,
    settings_path: String,
}

impl SettingsManager {
    pub fn new() -> Self {
        let settings_path = Self::get_settings_path();
        let settings = Self::load_settings(&settings_path).unwrap_or_default();
        
        Self {
            settings,
            settings_path,
        }
    }
    
    fn get_settings_path() -> String {
        if let Some(config_dir) = dirs::config_dir() {
            let prism_dir = config_dir.join("PrismLauncher");
            if !prism_dir.exists() {
                let _ = fs::create_dir_all(&prism_dir);
            }
            prism_dir.join("settings.json").to_string_lossy().to_string()
        } else {
            "settings.json".to_string()
        }
    }
    
    fn load_settings(path: &str) -> Result<AppSettings, String> {
        if Path::new(path).exists() {
            let content = fs::read_to_string(path)
                .map_err(|e| format!("Failed to read settings file: {}", e))?;
            let settings: AppSettings = serde_json::from_str(&content)
                .map_err(|e| format!("Failed to parse settings: {}", e))?;
            Ok(settings)
        } else {
            Ok(AppSettings::default())
        }
    }
    
    fn save_settings(&self) -> Result<(), String> {
        let content = serde_json::to_string_pretty(&self.settings)
            .map_err(|e| format!("Failed to serialize settings: {}", e))?;
        fs::write(&self.settings_path, content)
            .map_err(|e| format!("Failed to write settings file: {}", e))?;
        Ok(())
    }
    
    pub async fn get_settings(&self) -> Result<AppSettings, String> {
        Ok(self.settings.clone())
    }
    
    pub async fn set_settings(&mut self, settings: AppSettings) -> Result<bool, String> {
        self.settings = settings;
        self.save_settings()?;
        Ok(true)
    }
    
    pub async fn update_settings(&mut self, updates: HashMap<String, String>) -> Result<bool, String> {
        // 更新特定设置
        for (key, value) in updates {
            match key.as_str() {
                "theme" => self.settings.theme = value,
                "language" => self.settings.language = value,
                "auto_update" => self.settings.auto_update = value.parse().unwrap_or(true),
                "minimize_to_tray" => self.settings.minimize_to_tray = value.parse().unwrap_or(true),
                "start_minimized" => self.settings.start_minimized = value.parse().unwrap_or(false),
                "java_path" => self.settings.java_path = if value.is_empty() { None } else { Some(value) },
                "memory_size" => self.settings.memory_size = value.parse().unwrap_or(4096),
                "window_width" => self.settings.window_width = value.parse().unwrap_or(1200),
                "window_height" => self.settings.window_height = value.parse().unwrap_or(800),
                _ => {
                    self.settings.custom_settings.insert(key, value);
                }
            }
        }
        
        self.save_settings()?;
        Ok(true)
    }
    
    pub async fn reset_settings(&mut self) -> Result<bool, String> {
        self.settings = AppSettings::default();
        self.save_settings()?;
        Ok(true)
    }
    
    pub async fn get_setting(&self, key: &str) -> Result<Option<String>, String> {
        match key {
            "theme" => Ok(Some(self.settings.theme.clone())),
            "language" => Ok(Some(self.settings.language.clone())),
            "auto_update" => Ok(Some(self.settings.auto_update.to_string())),
            "minimize_to_tray" => Ok(Some(self.settings.minimize_to_tray.to_string())),
            "start_minimized" => Ok(Some(self.settings.start_minimized.to_string())),
            "java_path" => Ok(self.settings.java_path.clone()),
            "memory_size" => Ok(Some(self.settings.memory_size.to_string())),
            "window_width" => Ok(Some(self.settings.window_width.to_string())),
            "window_height" => Ok(Some(self.settings.window_height.to_string())),
            _ => Ok(self.settings.custom_settings.get(key).cloned()),
        }
    }
    
    pub async fn set_setting(&mut self, key: &str, value: String) -> Result<bool, String> {
        match key {
            "theme" => self.settings.theme = value,
            "language" => self.settings.language = value,
            "auto_update" => self.settings.auto_update = value.parse().unwrap_or(true),
            "minimize_to_tray" => self.settings.minimize_to_tray = value.parse().unwrap_or(true),
            "start_minimized" => self.settings.start_minimized = value.parse().unwrap_or(false),
            "java_path" => self.settings.java_path = if value.is_empty() { None } else { Some(value) },
            "memory_size" => self.settings.memory_size = value.parse().unwrap_or(4096),
            "window_width" => self.settings.window_width = value.parse().unwrap_or(1200),
            "window_height" => self.settings.window_height = value.parse().unwrap_or(800),
            _ => {
                self.settings.custom_settings.insert(key.to_string(), value);
            }
        }
        
        self.save_settings()?;
        Ok(true)
    }
}