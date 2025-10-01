#pragma once

#include "CreateInstanceDialog.g.h"

namespace winrt::PrismLauncherWinUI3::implementation
{
    class CreateInstanceDialog : public CreateInstanceDialogT<CreateInstanceDialog>
    {
    public:
        CreateInstanceDialog();

        // 属性
        hstring InstanceName() const { return m_instanceName; }
        hstring Version() const { return m_version; }
        hstring Group() const { return m_group; }
        hstring IconKey() const { return m_iconKey; }
        bool CreateWorld() const { return m_createWorld; }
        bool InstallForge() const { return m_installForge; }
        bool InstallFabric() const { return m_installFabric; }

    private:
        void InitializeComponent();
        void OnPrimaryButtonClick(Microsoft::UI::Xaml::Controls::ContentDialog const&, Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&);
        void OnSecondaryButtonClick(Microsoft::UI::Xaml::Controls::ContentDialog const&, Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&);
        void IconButton_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        // 成员变量
        hstring m_instanceName;
        hstring m_version;
        hstring m_group;
        hstring m_iconKey;
        bool m_createWorld{ false };
        bool m_installForge{ false };
        bool m_installFabric{ false };

        // UI控件引用
        Microsoft::UI::Xaml::Controls::TextBox m_instanceNameTextBox{ nullptr };
        Microsoft::UI::Xaml::Controls::ComboBox m_versionComboBox{ nullptr };
        Microsoft::UI::Xaml::Controls::ComboBox m_groupComboBox{ nullptr };
        Microsoft::UI::Xaml::Controls::Button m_iconButton{ nullptr };
        Microsoft::UI::Xaml::Controls::CheckBox m_createWorldCheckBox{ nullptr };
        Microsoft::UI::Xaml::Controls::CheckBox m_installForgeCheckBox{ nullptr };
        Microsoft::UI::Xaml::Controls::CheckBox m_installFabricCheckBox{ nullptr };
        Microsoft::UI::Xaml::Controls::TextBlock m_statusText{ nullptr };
    };
}

namespace winrt::PrismLauncherWinUI3::factory_implementation
{
    class CreateInstanceDialog : public CreateInstanceDialogT<CreateInstanceDialog, implementation::CreateInstanceDialog>
    {
    };
}