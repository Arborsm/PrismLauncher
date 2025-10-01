#pragma once

#include "App.g.h"
#include "PrismLauncherBridge.h"

namespace winrt::PrismLauncherWinUI3::implementation
{
    class App : public AppT<App>
    {
    public:
        App();
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        std::unique_ptr<PrismLauncherBridge> m_bridge;
    };
}

namespace winrt::PrismLauncherWinUI3::factory_implementation
{
    class App : public AppT<App, implementation::App>
    {
    };
}