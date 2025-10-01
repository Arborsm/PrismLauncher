use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccountInfo {
    pub id: String,
    pub username: String,
    pub uuid: String,
    pub account_type: String, // "microsoft", "offline"
    pub is_valid: bool,
    pub properties: HashMap<String, String>,
}

impl AccountInfo {
    pub fn new(username: String, account_type: String) -> Self {
        Self {
            id: Uuid::new_v4().to_string(),
            username,
            uuid: Uuid::new_v4().to_string(),
            account_type,
            is_valid: true,
            properties: HashMap::new(),
        }
    }
}

pub struct AccountManager {
    accounts: Vec<AccountInfo>,
}

impl AccountManager {
    pub fn new() -> Self {
        Self {
            accounts: Vec::new(),
        }
    }
    
    pub async fn get_accounts(&self) -> Result<Vec<AccountInfo>, String> {
        Ok(self.accounts.clone())
    }
    
    pub async fn get_account(&self, id: &str) -> Result<Option<AccountInfo>, String> {
        Ok(self.accounts.iter().find(|a| a.id == id).cloned())
    }
    
    pub async fn add_account(&mut self, username: String, account_type: String) -> Result<String, String> {
        let account = AccountInfo::new(username, account_type);
        let account_id = account.id.clone();
        
        self.accounts.push(account);
        Ok(account_id)
    }
    
    pub async fn remove_account(&mut self, id: String) -> Result<bool, String> {
        let initial_len = self.accounts.len();
        self.accounts.retain(|a| a.id != id);
        Ok(self.accounts.len() < initial_len)
    }
    
    pub async fn login_account(&mut self, id: String) -> Result<bool, String> {
        if let Some(account) = self.accounts.iter_mut().find(|a| a.id == id) {
            account.is_valid = true;
            Ok(true)
        } else {
            Err("Account not found".to_string())
        }
    }
    
    pub async fn logout_account(&mut self, id: String) -> Result<bool, String> {
        if let Some(account) = self.accounts.iter_mut().find(|a| a.id == id) {
            account.is_valid = false;
            Ok(true)
        } else {
            Err("Account not found".to_string())
        }
    }
    
    pub async fn update_account(&mut self, id: String, updates: HashMap<String, String>) -> Result<bool, String> {
        if let Some(account) = self.accounts.iter_mut().find(|a| a.id == id) {
            if let Some(username) = updates.get("username") {
                account.username = username.clone();
            }
            if let Some(uuid) = updates.get("uuid") {
                account.uuid = uuid.clone();
            }
            if let Some(account_type) = updates.get("account_type") {
                account.account_type = account_type.clone();
            }
            
            Ok(true)
        } else {
            Err("Account not found".to_string())
        }
    }
}