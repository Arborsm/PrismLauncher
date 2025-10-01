#include "pch.h"
#include "BooleanToVisibilityConverter.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Data;

namespace winrt::PrismLauncherWinUI3::Converters::implementation
{
    BooleanToVisibilityConverter::BooleanToVisibilityConverter()
    {
    }

    IInspectable BooleanToVisibilityConverter::Convert(IInspectable const& value, Microsoft::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        if (auto boolValue = value.try_as<bool>())
        {
            return box_value(boolValue ? Visibility::Visible : Visibility::Collapsed);
        }
        return box_value(Visibility::Collapsed);
    }

    IInspectable BooleanToVisibilityConverter::ConvertBack(IInspectable const& value, Microsoft::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        if (auto visibility = value.try_as<Visibility>())
        {
            return box_value(visibility == Visibility::Visible);
        }
        return box_value(false);
    }
}