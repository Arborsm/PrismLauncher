#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;

namespace winrt::PrismLauncherWinUI3::implementation
{
    App::App()
    {
        InitializeComponent();
        
        // 初始化桥接器
        m_bridge = std::make_unique<PrismLauncherBridge>();
        if (!m_bridge->Initialize())
        {
            // 处理初始化失败
        }
    }

    void App::OnLaunched(LaunchActivatedEventArgs const&)
    {
        // 创建主窗口
        window = make<MainWindow>();
        window.Activate();
    }
}