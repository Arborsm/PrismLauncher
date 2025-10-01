import React from 'react';
import {
  Nav,
  NavItem,
  NavCategory,
  makeStyles,
  tokens,
} from '@fluentui/react-components';
import {
  HomeRegular,
  GamepadRegular,
  PersonRegular,
  SettingsRegular,
  DownloadRegular,
  InfoRegular,
} from '@fluentui/react-icons';
import { useNavigate, useLocation } from 'react-router-dom';

const useStyles = makeStyles({
  sidebar: {
    height: '100%',
    padding: '16px 0',
  },
  navItem: {
    margin: '4px 0',
  },
});

export const Sidebar: React.FC = () => {
  const styles = useStyles();
  const navigate = useNavigate();
  const location = useLocation();

  const navItems = [
    {
      id: 'home',
      label: 'Home',
      icon: <HomeRegular />,
      path: '/',
    },
    {
      id: 'instances',
      label: 'Instances',
      icon: <GamepadRegular />,
      path: '/instances',
    },
    {
      id: 'accounts',
      label: 'Accounts',
      icon: <PersonRegular />,
      path: '/accounts',
    },
    {
      id: 'mods',
      label: 'Mods',
      icon: <DownloadRegular />,
      path: '/mods',
    },
    {
      id: 'settings',
      label: 'Settings',
      icon: <SettingsRegular />,
      path: '/settings',
    },
    {
      id: 'about',
      label: 'About',
      icon: <InfoRegular />,
      path: '/about',
    },
  ];

  const handleNavClick = (path: string) => {
    navigate(path);
  };

  return (
    <div className={styles.sidebar}>
      <Nav>
        <NavCategory>
          {navItems.map((item) => (
            <NavItem
              key={item.id}
              className={styles.navItem}
              icon={item.icon}
              onClick={() => handleNavClick(item.path)}
              value={item.path}
            >
              {item.label}
            </NavItem>
          ))}
        </NavCategory>
      </Nav>
    </div>
  );
};