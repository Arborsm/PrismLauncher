import React from 'react';
import {
  Button,
  makeStyles,
  tokens,
  Text,
  Avatar,
  Menu,
  MenuTrigger,
  MenuPopover,
  MenuList,
  MenuItem,
  MenuDivider,
} from '@fluentui/react-components';
import {
  PanelLeftExpandRegular,
  SettingsRegular,
  PersonRegular,
  SignOutRegular,
  InfoRegular,
} from '@fluentui/react-icons';
import { useAppStore } from '@/stores/appStore';

const useStyles = makeStyles({
  header: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '0 16px',
    height: '48px',
    borderBottom: `1px solid ${tokens.colorNeutralStroke1}`,
    backgroundColor: tokens.colorNeutralBackground1,
  },
  left: {
    display: 'flex',
    alignItems: 'center',
    gap: '12px',
  },
  right: {
    display: 'flex',
    alignItems: 'center',
    gap: '12px',
  },
  title: {
    fontSize: '16px',
    fontWeight: tokens.fontWeightSemibold,
    color: tokens.colorNeutralForeground1,
  },
  userInfo: {
    display: 'flex',
    alignItems: 'center',
    gap: '8px',
  },
});

interface HeaderProps {
  onToggleSidebar: () => void;
}

export const Header: React.FC<HeaderProps> = ({ onToggleSidebar }) => {
  const styles = useStyles();
  const { theme, setTheme, systemInfo } = useAppStore();

  const handleThemeToggle = () => {
    setTheme(theme === 'dark' ? 'light' : 'dark');
  };

  const handleSettings = () => {
    // Navigate to settings
    console.log('Open settings');
  };

  const handleAbout = () => {
    // Show about dialog
    console.log('Show about');
  };

  const handleSignOut = () => {
    // Sign out logic
    console.log('Sign out');
  };

  return (
    <header className={styles.header}>
      <div className={styles.left}>
        <Button
          appearance="subtle"
          icon={<PanelLeftExpandRegular />}
          onClick={onToggleSidebar}
        />
        <Text className={styles.title}>Prism Launcher</Text>
      </div>
      
      <div className={styles.right}>
        <Button
          appearance="subtle"
          icon={<SettingsRegular />}
          onClick={handleSettings}
        />
        
        <Menu>
          <MenuTrigger>
            <Button appearance="subtle" icon={<PersonRegular />} />
          </MenuTrigger>
          <MenuPopover>
            <MenuList>
              <MenuItem icon={<PersonRegular />}>
                Profile
              </MenuItem>
              <MenuItem icon={<SettingsRegular />} onClick={handleSettings}>
                Settings
              </MenuItem>
              <MenuDivider />
              <MenuItem icon={<InfoRegular />} onClick={handleAbout}>
                About
              </MenuItem>
              <MenuItem icon={<SignOutRegular />} onClick={handleSignOut}>
                Sign Out
              </MenuItem>
            </MenuList>
          </MenuPopover>
        </Menu>
      </div>
    </header>
  );
};