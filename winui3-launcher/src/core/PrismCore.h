#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>

#include <memory>
#include <string>
#include <vector>
#include <functional>

// 前向声明
class QApplication;
class Application;
class InstanceList;
class BaseInstance;

namespace winrt::PrismLauncherWinUI3::implementation
{
    // 简化的核心包装器，用于隔离Qt依赖
    class PrismCore
    {
    public:
        PrismCore();
        ~PrismCore();

        // 初始化
        bool Initialize();
        void Shutdown();

        // 获取应用实例
        Application* GetApplication() const { return m_app.get(); }
        InstanceList* GetInstanceList() const { return m_instanceList.get(); }

        // 检查是否已初始化
        bool IsInitialized() const { return m_initialized; }

    private:
        bool m_initialized;
        std::unique_ptr<QApplication> m_qtApp;
        std::unique_ptr<Application> m_app;
        std::shared_ptr<InstanceList> m_instanceList;
    };
}