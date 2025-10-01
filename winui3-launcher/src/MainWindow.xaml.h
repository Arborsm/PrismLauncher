#pragma once

#include "MainWindow.g.h"
#include "PrismLauncherBridge.h"
#include "Models/InstanceModel.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::PrismLauncherWinUI3::implementation
{
    class MainWindow : public MainWindowT<MainWindow>
    {
    public:
        MainWindow();

        // 公共方法
        void LoadInstances();
        void RefreshInstanceList();
        void ShowInstanceDetails(hstring const& instanceId);
        void HideInstanceDetails();

        // 属性
        Windows::Foundation::Collections::IObservableVector<winrt::PrismLauncherWinUI3::Models::InstanceModel> InstanceModels() const { return m_instanceModels; }

    private:
        void InitializeComponent();
        void UpdateInstanceList();
        void OnInstanceSelected(winrt::PrismLauncherWinUI3::Models::InstanceModel const& instance);
        void OnInstanceListChanged();
        void OnInstanceStatusChanged(const std::wstring& instanceId, bool isRunning);
        void OnLaunchStatusChanged(const std::wstring& instanceId, bool success, const std::wstring& error);
        void UpdateStatusBar();
        void UpdateToolbar();

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
        Windows::Foundation::Collections::IObservableVector<winrt::PrismLauncherWinUI3::Models::InstanceModel> m_instanceModels;
        winrt::PrismLauncherWinUI3::Models::InstanceModel m_selectedInstance{ nullptr };
        bool m_instanceSelected{ false };
        
        // UI控件引用
        Microsoft::UI::Xaml::Controls::ListView m_instanceListView{ nullptr };
        Microsoft::UI::Xaml::Controls::StackPanel m_welcomePanel{ nullptr };
        Microsoft::UI::Xaml::Controls::Grid m_instanceDetailsPanel{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_instanceNameText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_instanceVersionText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_instanceLastPlayedText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_totalPlayTimeText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_launchCountText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_lastLaunchText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_statusText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_instanceCountText{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_runningCountText{ nullptr };
    };
}

namespace winrt::PrismLauncherWinUI3::factory_implementation
{
    class MainWindow : public MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}