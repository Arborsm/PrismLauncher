#include "pch.h"
#include "MainWindow.xaml.h"
#include "App.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;

namespace winrt::PrismLauncherWinUI3::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        
        // 初始化桥接器
        m_bridge = std::make_unique<PrismLauncherBridge>();
        if (!m_bridge->Initialize())
        {
            // 处理初始化失败
            return;
        }

        // 设置事件回调
        m_bridge->SetInstanceListChangedCallback([this]() {
            this->OnInstanceListChanged();
        });

        m_bridge->SetInstanceStatusChangedCallback([this](const std::wstring& instanceId, bool isRunning) {
            this->OnInstanceStatusChanged(instanceId, isRunning);
        });

        m_bridge->SetLaunchStatusCallback([this](const std::wstring& instanceId, bool success, const std::wstring& error) {
            this->OnLaunchStatusChanged(instanceId, success, error);
        });

        // 加载实例列表
        LoadInstances();
    }

    void MainWindow::InitializeComponent()
    {
        MainWindowT::InitializeComponent();
    }

    void MainWindow::LoadInstances()
    {
        if (!m_bridge)
            return;

        m_instances = m_bridge->GetInstances();
        UpdateInstanceList();
    }

    void MainWindow::UpdateInstanceList()
    {
        // 更新UI中的实例列表
        // 这里需要将m_instances绑定到ListView
    }

    void MainWindow::OnInstanceSelected(const InstanceInfo& instance)
    {
        m_selectedInstance = instance;
        m_instanceSelected = true;

        // 更新UI显示选中的实例信息
        if (InstanceNameText())
        {
            InstanceNameText().Text(instance.name);
        }
        if (InstanceVersionText())
        {
            InstanceVersionText().Text(instance.version);
        }
        if (InstanceLastPlayedText())
        {
            InstanceLastPlayedText().Text(instance.lastPlayed);
        }

        // 显示实例详情面板
        if (WelcomePanel())
        {
            WelcomePanel().Visibility(Visibility::Collapsed);
        }
        if (InstanceDetailsPanel())
        {
            InstanceDetailsPanel().Visibility(Visibility::Visible);
        }
    }

    void MainWindow::OnInstanceListChanged()
    {
        LoadInstances();
    }

    void MainWindow::OnInstanceStatusChanged(const std::wstring& instanceId, bool isRunning)
    {
        // 更新实例状态
        for (auto& instance : m_instances)
        {
            if (instance.id == instanceId)
            {
                instance.isRunning = isRunning;
                break;
            }
        }
        UpdateInstanceList();
    }

    void MainWindow::OnLaunchStatusChanged(const std::wstring& instanceId, bool success, const std::wstring& error)
    {
        // 处理启动状态变化
        if (success)
        {
            // 启动成功
        }
        else
        {
            // 显示错误信息
        }
    }

    void MainWindow::AddInstance_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开添加实例对话框
    }

    void MainWindow::Settings_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开设置对话框
    }

    void MainWindow::CreateInstance_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开创建实例对话框
    }

    void MainWindow::InstanceListView_ItemClick(IInspectable const&, ItemClickEventArgs const&)
    {
        // 处理实例列表项点击
    }

    void MainWindow::Launch_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        LaunchParameters params;
        params.instanceId = m_selectedInstance.id;
        params.online = true;
        params.demo = false;

        m_bridge->LaunchInstance(params);
    }

    void MainWindow::Edit_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        // 打开编辑实例对话框
    }

    void MainWindow::Copy_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        // 实现复制实例功能
    }

    void MainWindow::Delete_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        // 确认删除对话框
        m_bridge->DeleteInstance(m_selectedInstance.id);
        LoadInstances();
    }

    void MainWindow::Export_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        // 打开导出实例对话框
    }

    void MainWindow::ViewLogs_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        // 打开日志查看器
    }
}