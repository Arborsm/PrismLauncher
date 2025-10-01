#include "pch.h"
#include "NullToVisibilityConverter.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Data;

namespace winrt::PrismLauncherWinUI3::Converters::implementation
{
    NullToVisibilityConverter::NullToVisibilityConverter()
    {
    }

    IInspectable NullToVisibilityConverter::Convert(IInspectable const& value, Microsoft::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, IInspectable const& language)
    {
        if (value == nullptr)
        {
            return box_value(Visibility::Collapsed);
        }
        return box_value(Visibility::Visible);
    }

    IInspectable NullToVisibilityConverter::ConvertBack(IInspectable const& value, Microsoft::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        if (auto visibility = value.try_as<Visibility>())
        {
            return box_value(visibility == Visibility::Visible ? nullptr : box_value(L""));
        }
        return nullptr;
    }
}