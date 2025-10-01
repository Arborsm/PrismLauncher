#include "pch.h"
#include "InstanceModel.h"
#include "PrismLauncherBridge.h"
#include <winrt/Microsoft.UI.Xaml.Media.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Media;

namespace winrt::PrismLauncherWinUI3::Models::implementation
{
    InstanceModel::InstanceModel()
    {
        // 初始化默认值
        Id = L"";
        Name = L"";
        Version = L"";
        LastPlayed = L"从未";
        IconKey = L"default";
        Group = L"默认";
        StatusText = L"就绪";
        TotalPlayTime = L"0 小时";
        LaunchCount = L"0";
        LastLaunch = L"从未";
        
        IsRunning = false;
        IsBroken = false;
        HasUpdate = false;
        IsSelected = false;
        
        TotalPlayTimeMs = 0;
        LastLaunchMs = 0;
        LaunchCountInt = 0;
    }

    void InstanceModel::UpdateFromInfo(winrt::PrismLauncherWinUI3::implementation::InstanceInfo const& info)
    {
        Id = info.id;
        Name = info.name;
        Version = info.version;
        LastPlayed = info.lastPlayed;
        IconKey = info.iconKey;
        Group = info.group;
        TotalPlayTimeMs = info.totalPlayTime;
        LastLaunchMs = info.lastLaunch;
        
        IsRunning = info.isRunning;
        IsBroken = info.isBroken;
        HasUpdate = info.hasUpdate;
        
        // 更新状态文本
        if (IsRunning)
        {
            StatusText = L"运行中";
        }
        else if (IsBroken)
        {
            StatusText = L"已损坏";
        }
        else if (HasUpdate)
        {
            StatusText = L"有更新";
        }
        else
        {
            StatusText = L"就绪";
        }
        
        // 格式化总游戏时间
        if (TotalPlayTimeMs > 0)
        {
            auto hours = TotalPlayTimeMs / (1000 * 60 * 60);
            auto minutes = (TotalPlayTimeMs % (1000 * 60 * 60)) / (1000 * 60);
            
            if (hours > 0)
            {
                TotalPlayTime = std::to_wstring(hours) + L" 小时 " + std::to_wstring(minutes) + L" 分钟";
            }
            else
            {
                TotalPlayTime = std::to_wstring(minutes) + L" 分钟";
            }
        }
        else
        {
            TotalPlayTime = L"0 小时";
        }
        
        // 格式化最后启动时间
        if (LastLaunchMs > 0)
        {
            auto now = std::chrono::system_clock::now();
            auto lastLaunch = std::chrono::system_clock::from_time_t(LastLaunchMs / 1000);
            auto diff = now - lastLaunch;
            auto days = std::chrono::duration_cast<std::chrono::days>(diff).count();
            
            if (days > 0)
            {
                LastLaunch = std::to_wstring(days) + L" 天前";
            }
            else
            {
                auto hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
                if (hours > 0)
                {
                    LastLaunch = std::to_wstring(hours) + L" 小时前";
                }
                else
                {
                    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
                    LastLaunch = std::to_wstring(minutes) + L" 分钟前";
                }
            }
        }
        else
        {
            LastLaunch = L"从未";
        }
        
        // 更新启动次数
        LaunchCount = std::to_wstring(LaunchCountInt);
        
        // 触发属性更改通知
        NotifyPropertyChanged(L"StatusText");
        NotifyPropertyChanged(L"TotalPlayTime");
        NotifyPropertyChanged(L"LastLaunch");
        NotifyPropertyChanged(L"LaunchCount");
        NotifyPropertyChanged(L"StatusBrush");
        NotifyPropertyChanged(L"IconBrush");
        NotifyPropertyChanged(L"BackgroundBrush");
    }

    void InstanceModel::NotifyPropertyChanged(hstring const& propertyName)
    {
        m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }

    Microsoft::UI::Xaml::Media::Brush InstanceModel::StatusBrush()
    {
        if (IsRunning)
        {
            return SolidColorBrush(Colors::Green());
        }
        else if (IsBroken)
        {
            return SolidColorBrush(Colors::Red());
        }
        else if (HasUpdate)
        {
            return SolidColorBrush(Colors::Orange());
        }
        else
        {
            return SolidColorBrush(Colors::Gray());
        }
    }

    Microsoft::UI::Xaml::Media::Brush InstanceModel::IconBrush()
    {
        // 根据图标键返回不同的颜色
        if (IconKey == L"default")
        {
            return SolidColorBrush(Colors::Purple());
        }
        else if (IconKey == L"grass")
        {
            return SolidColorBrush(Colors::Green());
        }
        else if (IconKey == L"dirt")
        {
            return SolidColorBrush(Colors::Brown());
        }
        else
        {
            return SolidColorBrush(Colors::Blue());
        }
    }

    Microsoft::UI::Xaml::Media::Brush InstanceModel::BackgroundBrush()
    {
        if (IsSelected)
        {
            return SolidColorBrush(Colors::Purple());
        }
        else if (IsRunning)
        {
            return SolidColorBrush(Colors::DarkGreen());
        }
        else
        {
            return SolidColorBrush(Colors::Transparent());
        }
    }
}