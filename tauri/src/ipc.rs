use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

// IPC消息类型
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum MessageType {
    Request = 0,
    Response = 1,
    Notification = 2,
    Error = 3,
}

// 服务方法枚举
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ServiceMethod {
    // Instance management
    CreateInstance = 1000,
    DeleteInstance = 1001,
    CopyInstance = 1002,
    ListInstances = 1003,
    GetInstanceInfo = 1004,
    UpdateInstance = 1005,
    
    // Account management
    ListAccounts = 3000,
    AddAccount = 3001,
    RemoveAccount = 3002,
    LoginAccount = 3003,
    LogoutAccount = 3004,
    GetAccountInfo = 3005,
    
    // Java management
    ListJavaInstalls = 4000,
    GetJavaInfo = 4001,
    InstallJava = 4002,
    UninstallJava = 4003,
    CheckJavaCompatibility = 4004,
    
    // Launch control
    LaunchInstance = 5000,
    StopInstance = 5001,
    GetLaunchStatus = 5002,
    GetLaunchLogs = 5003,
    
    // Settings
    GetSettings = 6000,
    SetSettings = 6001,
    ResetSettings = 6002,
    
    // Mod platform
    SearchMods = 7000,
    GetModInfo = 7001,
    InstallMod = 7002,
    UninstallMod = 7003,
    UpdateMod = 7004,
    
    // System info
    GetSystemInfo = 8000,
    GetLauncherInfo = 8001,
    
    // File operations
    OpenFile = 9000,
    OpenFolder = 9001,
    CopyFile = 9002,
    DeleteFile = 9003,
}

// 状态枚举
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum Status {
    Success = 0,
    Error = 1,
    Timeout = 2,
    InvalidRequest = 3,
    NotFound = 4,
    Unauthorized = 5,
}

// IPC请求结构
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IPCRequest {
    pub message_type: MessageType,
    pub method: ServiceMethod,
    pub request_id: String,
    pub headers: HashMap<String, String>,
    pub parameters: HashMap<String, String>,
    pub payload: String,
}

impl IPCRequest {
    pub fn new(method: ServiceMethod) -> Self {
        Self {
            message_type: MessageType::Request,
            method,
            request_id: Uuid::new_v4().to_string(),
            headers: HashMap::new(),
            parameters: HashMap::new(),
            payload: String::new(),
        }
    }
    
    pub fn with_params(method: ServiceMethod, params: HashMap<String, String>) -> Self {
        Self {
            message_type: MessageType::Request,
            method,
            request_id: Uuid::new_v4().to_string(),
            headers: HashMap::new(),
            parameters: params,
            payload: String::new(),
        }
    }
}

// IPC响应结构
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IPCResponse {
    pub message_type: MessageType,
    pub method: ServiceMethod,
    pub request_id: String,
    pub status: Status,
    pub error_message: String,
    pub data: String,
    pub headers: HashMap<String, String>,
}

impl IPCResponse {
    pub fn success(method: ServiceMethod, request_id: String, data: String) -> Self {
        Self {
            message_type: MessageType::Response,
            method,
            request_id,
            status: Status::Success,
            error_message: String::new(),
            data,
            headers: HashMap::new(),
        }
    }
    
    pub fn error(method: ServiceMethod, request_id: String, error_message: String) -> Self {
        Self {
            message_type: MessageType::Response,
            method,
            request_id,
            status: Status::Error,
            error_message,
            data: String::new(),
            headers: HashMap::new(),
        }
    }
}

// IPC服务
pub struct IPCService {
    // 可以添加内部状态
}

impl IPCService {
    pub fn new() -> Self {
        Self {}
    }
    
    pub async fn process_request(&self, request: IPCRequest) -> Result<IPCResponse, String> {
        // 根据请求方法处理请求
        match request.method {
            ServiceMethod::ListInstances => {
                // 处理列出实例的请求
                Ok(IPCResponse::success(
                    request.method,
                    request.request_id,
                    "[]".to_string(), // 实际实现中会返回实例列表
                ))
            }
            ServiceMethod::CreateInstance => {
                // 处理创建实例的请求
                Ok(IPCResponse::success(
                    request.method,
                    request.request_id,
                    "instance_id".to_string(),
                ))
            }
            ServiceMethod::GetSystemInfo => {
                // 处理获取系统信息的请求
                Ok(IPCResponse::success(
                    request.method,
                    request.request_id,
                    r#"{"osName":"Windows","osVersion":"10","architecture":"x64","javaVersion":"17.0.2","launcherVersion":"1.0.0"}"#.to_string(),
                ))
            }
            _ => {
                // 默认处理
                Ok(IPCResponse::error(
                    request.method,
                    request.request_id,
                    "Method not implemented".to_string(),
                ))
            }
        }
    }
}