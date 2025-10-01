#include "pch.h"
#include "MainWindow.xaml.h"
#include "App.xaml.h"
#include "Models/InstanceModel.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation::Collections;

namespace winrt::PrismLauncherWinUI3::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        
        // 初始化实例模型集合
        m_instanceModels = single_threaded_observable_vector<winrt::PrismLauncherWinUI3::Models::InstanceModel>();
        
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

        // 获取UI控件引用
        m_instanceListView = InstanceListView();
        m_welcomePanel = WelcomePanel();
        m_instanceDetailsPanel = InstanceDetailsPanel();
        m_instanceNameText = InstanceNameText();
        m_instanceVersionText = InstanceVersionText();
        m_instanceLastPlayedText = InstanceLastPlayedText();
        m_totalPlayTimeText = TotalPlayTimeText();
        m_launchCountText = LaunchCountText();
        m_lastLaunchText = LastLaunchText();
        m_statusText = StatusText();
        m_instanceCountText = InstanceCountText();
        m_runningCountText = RunningCountText();

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

        auto instances = m_bridge->GetInstances();
        m_instanceModels.Clear();

        for (const auto& instanceInfo : instances)
        {
            auto model = winrt::make<winrt::PrismLauncherWinUI3::Models::implementation::InstanceModel>();
            model.UpdateFromInfo(instanceInfo);
            m_instanceModels.Append(model);
        }

        UpdateStatusBar();
    }

    void MainWindow::RefreshInstanceList()
    {
        LoadInstances();
    }

    void MainWindow::ShowInstanceDetails(hstring const& instanceId)
    {
        // 查找对应的实例模型
        for (const auto& model : m_instanceModels)
        {
            if (model.Id() == instanceId)
            {
                OnInstanceSelected(model);
                break;
            }
        }
    }

    void MainWindow::HideInstanceDetails()
    {
        m_selectedInstance = nullptr;
        m_instanceSelected = false;

        if (m_welcomePanel)
        {
            m_welcomePanel.Visibility(Visibility::Visible);
        }
        if (m_instanceDetailsPanel)
        {
            m_instanceDetailsPanel.Visibility(Visibility::Collapsed);
        }

        UpdateToolbar();
    }

    void MainWindow::UpdateInstanceList()
    {
        // 更新UI中的实例列表
        // 数据绑定会自动处理
    }

    void MainWindow::OnInstanceSelected(winrt::PrismLauncherWinUI3::Models::InstanceModel const& instance)
    {
        m_selectedInstance = instance;
        m_instanceSelected = true;

        // 更新UI显示选中的实例信息
        if (m_instanceNameText)
        {
            m_instanceNameText.Text(instance.Name());
        }
        if (m_instanceVersionText)
        {
            m_instanceVersionText.Text(instance.Version());
        }
        if (m_instanceLastPlayedText)
        {
            m_instanceLastPlayedText.Text(instance.LastPlayed());
        }
        if (m_totalPlayTimeText)
        {
            m_totalPlayTimeText.Text(instance.TotalPlayTime());
        }
        if (m_launchCountText)
        {
            m_launchCountText.Text(instance.LaunchCount());
        }
        if (m_lastLaunchText)
        {
            m_lastLaunchText.Text(instance.LastLaunch());
        }

        // 显示实例详情面板
        if (m_welcomePanel)
        {
            m_welcomePanel.Visibility(Visibility::Collapsed);
        }
        if (m_instanceDetailsPanel)
        {
            m_instanceDetailsPanel.Visibility(Visibility::Visible);
        }

        UpdateToolbar();
    }

    void MainWindow::OnInstanceListChanged()
    {
        LoadInstances();
    }

    void MainWindow::OnInstanceStatusChanged(const std::wstring& instanceId, bool isRunning)
    {
        // 更新实例状态
        for (const auto& model : m_instanceModels)
        {
            if (model.Id() == instanceId)
            {
                model.IsRunning(isRunning);
                break;
            }
        }
        UpdateStatusBar();
    }

    void MainWindow::OnLaunchStatusChanged(const std::wstring& instanceId, bool success, const std::wstring& error)
    {
        // 处理启动状态变化
        if (success)
        {
            if (m_statusText)
            {
                m_statusText.Text(L"启动成功");
            }
        }
        else
        {
            if (m_statusText)
            {
                m_statusText.Text(L"启动失败: " + error);
            }
        }
    }

    void MainWindow::UpdateStatusBar()
    {
        if (!m_instanceCountText || !m_runningCountText)
            return;

        int totalCount = m_instanceModels.Size();
        int runningCount = 0;

        for (const auto& model : m_instanceModels)
        {
            if (model.IsRunning())
            {
                runningCount++;
            }
        }

        m_instanceCountText.Text(std::to_wstring(totalCount) + L" 个实例");
        m_runningCountText.Text(std::to_wstring(runningCount) + L" 个运行中");
    }

    void MainWindow::UpdateToolbar()
    {
        // 更新工具栏的可见性和状态
        // 这里可以根据选中的实例状态更新按钮
    }

    void MainWindow::AddInstance_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开添加实例对话框
        if (m_statusText)
        {
            m_statusText.Text(L"打开添加实例对话框...");
        }
    }

    void MainWindow::Settings_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开设置对话框
        if (m_statusText)
        {
            m_statusText.Text(L"打开设置对话框...");
        }
    }

    void MainWindow::CreateInstance_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开创建实例对话框
        if (m_statusText)
        {
            m_statusText.Text(L"打开创建实例对话框...");
        }
    }

    void MainWindow::InstanceListView_ItemClick(IInspectable const&, ItemClickEventArgs const& args)
    {
        // 处理实例列表项点击
        if (auto model = args.ClickedItem().try_as<winrt::PrismLauncherWinUI3::Models::InstanceModel>())
        {
            OnInstanceSelected(model);
        }
    }

    void MainWindow::Launch_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"正在启动实例...");
        }

        LaunchParameters params;
        params.instanceId = m_selectedInstance.Id();
        params.online = true;
        params.demo = false;

        m_bridge->LaunchInstance(params);
    }

    void MainWindow::Edit_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"打开编辑实例对话框...");
        }
    }

    void MainWindow::Copy_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"正在复制实例...");
        }

        // 实现复制实例功能
        m_bridge->CopyInstance(m_selectedInstance.Id(), m_selectedInstance.Name() + L" (副本)");
        LoadInstances();
    }

    void MainWindow::Delete_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected || !m_bridge)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"正在删除实例...");
        }

        // 确认删除对话框
        m_bridge->DeleteInstance(m_selectedInstance.Id());
        LoadInstances();
        HideInstanceDetails();
    }

    void MainWindow::Export_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"打开导出实例对话框...");
        }
    }

    void MainWindow::ViewLogs_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!m_instanceSelected)
            return;

        if (m_statusText)
        {
            m_statusText.Text(L"打开日志查看器...");
        }
    }
}