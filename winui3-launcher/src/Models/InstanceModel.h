#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <string>

namespace winrt::PrismLauncherWinUI3::Models
{
    [bindable]
    runtimeclass InstanceModel : winrt::Microsoft::UI::Xaml::Data::INotifyPropertyChanged
    {
        InstanceModel();
        
        String Id{ get; set; };
        String Name{ get; set; };
        String Version{ get; set; };
        String LastPlayed{ get; set; };
        String IconKey{ get; set; };
        String Group{ get; set; };
        String StatusText{ get; set; };
        String TotalPlayTime{ get; set; };
        String LaunchCount{ get; set; };
        String LastLaunch{ get; set; };
        
        Boolean IsRunning{ get; set; };
        Boolean IsBroken{ get; set; };
        Boolean HasUpdate{ get; set; };
        Boolean IsSelected{ get; set; };
        
        Int64 TotalPlayTimeMs{ get; set; };
        Int64 LastLaunchMs{ get; set; };
        Int32 LaunchCountInt{ get; set; };
        
        Microsoft::UI::Xaml::Media::Brush StatusBrush{ get; };
        Microsoft::UI::Xaml::Media::Brush IconBrush{ get; };
        Microsoft::UI::Xaml::Media::Brush BackgroundBrush{ get; };
        
        void UpdateFromInfo(winrt::PrismLauncherWinUI3::implementation::InstanceInfo const& info);
        void NotifyPropertyChanged(hstring const& propertyName);
    };
}