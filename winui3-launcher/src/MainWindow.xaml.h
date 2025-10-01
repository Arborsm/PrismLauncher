#pragma once

#include "MainWindow.g.h"
#include "PrismLauncherBridge.h"

namespace winrt::PrismLauncherWinUI3::implementation
{
    class MainWindow : public MainWindowT<MainWindow>
    {
    public:
        MainWindow();

    private:
        void InitializeComponent();
        void LoadInstances();
        void UpdateInstanceList();
        void OnInstanceSelected(const InstanceInfo& instance);
        void OnInstanceListChanged();
        void OnInstanceStatusChanged(const std::wstring& instanceId, bool isRunning);
        void OnLaunchStatusChanged(const std::wstring& instanceId, bool success, const std::wstring& error);

        // 事件处理器
        void AddInstance_Click(IInspectable const&, RoutedEventArgs const&);
        void Settings_Click(IInspectable const&, RoutedEventArgs const&);
        void CreateInstance_Click(IInspectable const&, RoutedEventArgs const&);
        void InstanceListView_ItemClick(IInspectable const&, ItemClickEventArgs const&);
        void Launch_Click(IInspectable const&, RoutedEventArgs const&);
        void Edit_Click(IInspectable const&, RoutedEventArgs const&);
        void Copy_Click(IInspectable const&, RoutedEventArgs const&);
        void Delete_Click(IInspectable const&, RoutedEventArgs const&);
        void Export_Click(IInspectable const&, RoutedEventArgs const&);
        void ViewLogs_Click(IInspectable const&, RoutedEventArgs const&);

        // 成员变量
        std::unique_ptr<PrismLauncherBridge> m_bridge;
        std::vector<InstanceInfo> m_instances;
        InstanceInfo m_selectedInstance;
        bool m_instanceSelected;
    };
}

namespace winrt::PrismLauncherWinUI3::factory_implementation
{
    class MainWindow : public MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}