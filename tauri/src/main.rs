// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::{Manager, State};
use std::sync::Mutex;
use std::collections::HashMap;
use serde::{Deserialize, Serialize};
use uuid::Uuid;
use chrono::{DateTime, Utc};

mod ipc;
mod instance;
mod account;
mod java;
mod system;
mod settings;
mod mod_platform;

use ipc::{IPCService, IPCRequest, IPCResponse, ServiceMethod, Status};
use instance::{InstanceInfo, InstanceManager};
use account::{AccountInfo, AccountManager};
use java::{JavaInfo, JavaManager};
use system::{SystemInfo, SystemManager};
use settings::{SettingsManager, AppSettings};

// 应用状态
struct AppState {
    ipc_service: Mutex<IPCService>,
    instance_manager: Mutex<InstanceManager>,
    account_manager: Mutex<AccountManager>,
    java_manager: Mutex<JavaManager>,
    system_manager: Mutex<SystemManager>,
    settings_manager: Mutex<SettingsManager>,
}

impl AppState {
    fn new() -> Self {
        Self {
            ipc_service: Mutex::new(IPCService::new()),
            instance_manager: Mutex::new(InstanceManager::new()),
            account_manager: Mutex::new(AccountManager::new()),
            java_manager: Mutex::new(JavaManager::new()),
            system_manager: Mutex::new(SystemManager::new()),
            settings_manager: Mutex::new(SettingsManager::new()),
        }
    }
}

// IPC处理函数
#[tauri::command]
async fn handle_ipc_request(
    state: State<'_, AppState>,
    request: IPCRequest,
) -> Result<IPCResponse, String> {
    let ipc_service = state.ipc_service.lock().unwrap();
    ipc_service.process_request(request).await
}

// 实例管理命令
#[tauri::command]
async fn get_instances(state: State<'_, AppState>) -> Result<Vec<InstanceInfo>, String> {
    let manager = state.instance_manager.lock().unwrap();
    manager.get_instances().await
}

#[tauri::command]
async fn create_instance(
    state: State<'_, AppState>,
    name: String,
    version: Option<String>,
) -> Result<String, String> {
    let manager = state.instance_manager.lock().unwrap();
    manager.create_instance(name, version.unwrap_or_else(|| "latest".to_string())).await
}

#[tauri::command]
async fn delete_instance(state: State<'_, AppState>, id: String) -> Result<bool, String> {
    let manager = state.instance_manager.lock().unwrap();
    manager.delete_instance(id).await
}

#[tauri::command]
async fn launch_instance(state: State<'_, AppState>, id: String) -> Result<bool, String> {
    let manager = state.instance_manager.lock().unwrap();
    manager.launch_instance(id).await
}

#[tauri::command]
async fn stop_instance(state: State<'_, AppState>, id: String) -> Result<bool, String> {
    let manager = state.instance_manager.lock().unwrap();
    manager.stop_instance(id).await
}

// 账户管理命令
#[tauri::command]
async fn get_accounts(state: State<'_, AppState>) -> Result<Vec<AccountInfo>, String> {
    let manager = state.account_manager.lock().unwrap();
    manager.get_accounts().await
}

#[tauri::command]
async fn add_account(
    state: State<'_, AppState>,
    username: String,
    account_type: String,
) -> Result<String, String> {
    let manager = state.account_manager.lock().unwrap();
    manager.add_account(username, account_type).await
}

#[tauri::command]
async fn remove_account(state: State<'_, AppState>, id: String) -> Result<bool, String> {
    let manager = state.account_manager.lock().unwrap();
    manager.remove_account(id).await
}

// Java管理命令
#[tauri::command]
async fn get_java_installs(state: State<'_, AppState>) -> Result<Vec<JavaInfo>, String> {
    let manager = state.java_manager.lock().unwrap();
    manager.get_java_installs().await
}

#[tauri::command]
async fn install_java(state: State<'_, AppState>, version: String) -> Result<bool, String> {
    let manager = state.java_manager.lock().unwrap();
    manager.install_java(version).await
}

// 系统信息命令
#[tauri::command]
async fn get_system_info(state: State<'_, AppState>) -> Result<SystemInfo, String> {
    let manager = state.system_manager.lock().unwrap();
    manager.get_system_info().await
}

// 设置管理命令
#[tauri::command]
async fn get_settings(state: State<'_, AppState>) -> Result<AppSettings, String> {
    let manager = state.settings_manager.lock().unwrap();
    manager.get_settings().await
}

#[tauri::command]
async fn set_settings(
    state: State<'_, AppState>,
    settings: AppSettings,
) -> Result<bool, String> {
    let manager = state.settings_manager.lock().unwrap();
    manager.set_settings(settings).await
}

// 文件操作命令
#[tauri::command]
async fn open_file(path: String) -> Result<(), String> {
    #[cfg(target_os = "windows")]
    {
        std::process::Command::new("cmd")
            .args(["/C", "start", "", &path])
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    #[cfg(target_os = "macos")]
    {
        std::process::Command::new("open")
            .arg(&path)
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    #[cfg(target_os = "linux")]
    {
        std::process::Command::new("xdg-open")
            .arg(&path)
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    Ok(())
}

#[tauri::command]
async fn open_folder(path: String) -> Result<(), String> {
    #[cfg(target_os = "windows")]
    {
        std::process::Command::new("explorer")
            .arg(&path)
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    #[cfg(target_os = "macos")]
    {
        std::process::Command::new("open")
            .arg(&path)
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    #[cfg(target_os = "linux")]
    {
        std::process::Command::new("xdg-open")
            .arg(&path)
            .spawn()
            .map_err(|e| e.to_string())?;
    }
    
    Ok(())
}

fn main() {
    env_logger::init();
    
    tauri::Builder::default()
        .manage(AppState::new())
        .invoke_handler(tauri::generate_handler![
            handle_ipc_request,
            get_instances,
            create_instance,
            delete_instance,
            launch_instance,
            stop_instance,
            get_accounts,
            add_account,
            remove_account,
            get_java_installs,
            install_java,
            get_system_info,
            get_settings,
            set_settings,
            open_file,
            open_folder
        ])
        .setup(|app| {
            // 初始化应用
            log::info!("Prism Launcher starting...");
            
            // 设置窗口标题
            if let Some(window) = app.get_window("main") {
                window.set_title("Prism Launcher").unwrap();
            }
            
            Ok(())
        })
        .on_window_event(|event| {
            match event.event() {
                tauri::WindowEvent::CloseRequested { .. } => {
                    log::info!("Application closing...");
                }
                _ => {}
            }
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}