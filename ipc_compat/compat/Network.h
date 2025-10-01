// SPDX-License-Identifier: GPL-3.0-only
/*
 * Network Compatibility Layer for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "String.h"
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

namespace Compat {

// HTTP方法
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS
};

// HTTP状态码
enum class HttpStatusCode {
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    InternalServerError = 500,
    BadGateway = 502,
    ServiceUnavailable = 503
};

// 网络请求类
class NetworkRequest {
public:
    NetworkRequest();
    NetworkRequest(const String& url);
    
    void setUrl(const String& url) { m_url = url; }
    String url() const { return m_url; }
    
    void setMethod(HttpMethod method) { m_method = method; }
    HttpMethod method() const { return m_method; }
    
    void setHeader(const String& name, const String& value);
    String header(const String& name) const;
    void removeHeader(const String& name);
    std::map<String, String> headers() const { return m_headers; }
    
    void setData(const String& data) { m_data = data; }
    String data() const { return m_data; }
    
    void setTimeout(int timeout) { m_timeout = timeout; }
    int timeout() const { return m_timeout; }

private:
    String m_url;
    HttpMethod m_method = HttpMethod::GET;
    std::map<String, String> m_headers;
    String m_data;
    int m_timeout = 30000; // 30秒
};

// 网络响应类
class NetworkResponse {
public:
    NetworkResponse();
    
    HttpStatusCode statusCode() const { return m_statusCode; }
    void setStatusCode(HttpStatusCode code) { m_statusCode = code; }
    
    String statusText() const { return m_statusText; }
    void setStatusText(const String& text) { m_statusText = text; }
    
    String header(const String& name) const;
    void setHeader(const String& name, const String& value);
    std::map<String, String> headers() const { return m_headers; }
    
    String body() const { return m_body; }
    void setBody(const String& body) { m_body = body; }
    
    String errorString() const { return m_errorString; }
    void setErrorString(const String& error) { m_errorString = error; }
    
    bool isSuccessful() const;

private:
    HttpStatusCode m_statusCode = HttpStatusCode::OK;
    String m_statusText;
    std::map<String, String> m_headers;
    String m_body;
    String m_errorString;
};

// 网络访问管理器
class NetworkAccessManager {
public:
    NetworkAccessManager();
    virtual ~NetworkAccessManager();
    
    // 发送请求
    std::future<NetworkResponse> get(const NetworkRequest& request);
    std::future<NetworkResponse> post(const NetworkRequest& request);
    std::future<NetworkResponse> put(const NetworkRequest& request);
    std::future<NetworkResponse> deleteResource(const NetworkRequest& request);
    
    // 异步请求
    void getAsync(const NetworkRequest& request, std::function<void(const NetworkResponse&)> callback);
    void postAsync(const NetworkRequest& request, std::function<void(const NetworkResponse&)> callback);
    
    // 设置代理
    void setProxy(const String& host, int port, const String& username = String(), const String& password = String());
    
    // 设置用户代理
    void setUserAgent(const String& userAgent) { m_userAgent = userAgent; }
    String userAgent() const { return m_userAgent; }
    
    // 设置超时
    void setTimeout(int timeout) { m_timeout = timeout; }
    int timeout() const { return m_timeout; }

private:
    NetworkResponse performRequest(const NetworkRequest& request);
    void performRequestAsync(const NetworkRequest& request, std::function<void(const NetworkResponse&)> callback);
    
    String m_userAgent = "PrismLauncher-IPC/1.0";
    int m_timeout = 30000;
    
    // 代理设置
    String m_proxyHost;
    int m_proxyPort = 0;
    String m_proxyUsername;
    String m_proxyPassword;
    bool m_useProxy = false;
};

// 下载管理器
class DownloadManager {
public:
    DownloadManager();
    virtual ~DownloadManager();
    
    // 下载文件
    std::future<bool> downloadFile(const String& url, const String& filePath);
    std::future<bool> downloadFile(const String& url, const String& filePath, 
                                  std::function<void(int64_t, int64_t)> progressCallback);
    
    // 异步下载
    void downloadFileAsync(const String& url, const String& filePath,
                          std::function<void(bool)> callback);
    void downloadFileAsync(const String& url, const String& filePath,
                          std::function<void(bool)> callback,
                          std::function<void(int64_t, int64_t)> progressCallback);
    
    // 取消下载
    void cancelDownload(const String& url);
    void cancelAllDownloads();
    
    // 设置并发下载数
    void setMaxConcurrentDownloads(int max) { m_maxConcurrentDownloads = max; }
    int maxConcurrentDownloads() const { return m_maxConcurrentDownloads; }

private:
    bool downloadFileInternal(const String& url, const String& filePath,
                             std::function<void(int64_t, int64_t)> progressCallback);
    
    int m_maxConcurrentDownloads = 5;
    std::map<String, std::atomic<bool>*> m_cancelFlags;
    std::mutex m_cancelMutex;
};

// URL类
class Url {
public:
    Url();
    Url(const String& url);
    
    void setUrl(const String& url);
    String url() const { return m_url; }
    
    String scheme() const { return m_scheme; }
    String host() const { return m_host; }
    int port() const { return m_port; }
    String path() const { return m_path; }
    String query() const { return m_query; }
    String fragment() const { return m_fragment; }
    
    bool isValid() const { return m_isValid; }
    
    String toString() const;

private:
    void parseUrl(const String& url);
    
    String m_url;
    String m_scheme;
    String m_host;
    int m_port = 0;
    String m_path;
    String m_query;
    String m_fragment;
    bool m_isValid = false;
};

} // namespace Compat