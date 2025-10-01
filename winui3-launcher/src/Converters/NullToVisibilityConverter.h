#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.h>

namespace winrt::PrismLauncherWinUI3::Converters
{
    [default_interface]
    runtimeclass NullToVisibilityConverter : winrt::Microsoft::UI::Xaml::Data::IValueConverter
    {
        NullToVisibilityConverter();
        Object Convert(Object value, Microsoft::UI::Xaml::Interop::TypeName targetType, Object parameter, hstring language);
        Object ConvertBack(Object value, Microsoft::UI::Xaml::Interop::TypeName targetType, Object parameter, hstring language);
    };
}