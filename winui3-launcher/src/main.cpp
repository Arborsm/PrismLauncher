#include "pch.h"
#include "App.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// 程序入口点
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    // 初始化WinRT
    winrt::init_apartment();

    // 创建应用实例
    Application::Start([](auto&&) {
        make<PrismLauncherWinUI3::App>();
    });

    return 0;
}