#include "pch.h"
#include "CreateInstanceDialog.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::PrismLauncherWinUI3::implementation
{
    CreateInstanceDialog::CreateInstanceDialog()
    {
        InitializeComponent();
        
        // 获取UI控件引用
        m_instanceNameTextBox = InstanceNameTextBox();
        m_versionComboBox = VersionComboBox();
        m_groupComboBox = GroupComboBox();
        m_iconButton = IconButton();
        m_createWorldCheckBox = CreateWorldCheckBox();
        m_installForgeCheckBox = InstallForgeCheckBox();
        m_installFabricCheckBox = InstallFabricCheckBox();
        m_statusText = StatusText();

        // 设置事件处理器
        PrimaryButtonClick({ this, &CreateInstanceDialog::OnPrimaryButtonClick });
        SecondaryButtonClick({ this, &CreateInstanceDialog::OnSecondaryButtonClick });
        m_iconButton.Click({ this, &CreateInstanceDialog::IconButton_Click });

        // 初始化默认值
        m_iconKey = L"default";
        m_group = L"默认";
    }

    void CreateInstanceDialog::InitializeComponent()
    {
        CreateInstanceDialogT::InitializeComponent();
    }

    void CreateInstanceDialog::OnPrimaryButtonClick(ContentDialog const&, ContentDialogButtonClickEventArgs const&)
    {
        // 验证输入
        if (m_instanceNameTextBox.Text().empty())
        {
            m_statusText.Text(L"请输入实例名称");
            return;
        }

        if (m_versionComboBox.SelectedItem() == nullptr)
        {
            m_statusText.Text(L"请选择Minecraft版本");
            return;
        }

        // 保存输入值
        m_instanceName = m_instanceNameTextBox.Text();
        m_version = m_versionComboBox.SelectedItem().as<hstring>();
        m_group = m_groupComboBox.SelectedItem() != nullptr ? 
                  m_groupComboBox.SelectedItem().as<hstring>() : L"默认";
        m_createWorld = m_createWorldCheckBox.IsChecked().GetValueOrDefault(false);
        m_installForge = m_installForgeCheckBox.IsChecked().GetValueOrDefault(false);
        m_installFabric = m_installFabricCheckBox.IsChecked().GetValueOrDefault(false);

        m_statusText.Text(L"正在创建实例...");
    }

    void CreateInstanceDialog::OnSecondaryButtonClick(ContentDialog const&, ContentDialogButtonClickEventArgs const&)
    {
        // 取消操作
    }

    void CreateInstanceDialog::IconButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // 打开图标选择对话框
        m_statusText.Text(L"选择图标...");
    }
}