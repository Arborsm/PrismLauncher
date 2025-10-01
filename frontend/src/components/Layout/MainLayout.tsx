import React, { useState } from 'react';
import {
  FluentProvider,
  webLightTheme,
  webDarkTheme,
  makeStyles,
  tokens,
} from '@fluentui/react-components';
import { Navigation } from './Navigation';
import { Header } from './Header';
import { Sidebar } from './Sidebar';
import { useAppStore } from '@/stores/appStore';

const useStyles = makeStyles({
  root: {
    display: 'flex',
    flexDirection: 'column',
    height: '100vh',
    backgroundColor: tokens.colorNeutralBackground1,
  },
  main: {
    display: 'flex',
    flex: 1,
    overflow: 'hidden',
  },
  content: {
    flex: 1,
    display: 'flex',
    flexDirection: 'column',
    overflow: 'hidden',
  },
  sidebar: {
    width: '280px',
    borderRight: `1px solid ${tokens.colorNeutralStroke1}`,
    backgroundColor: tokens.colorNeutralBackground1,
  },
});

interface MainLayoutProps {
  children: React.ReactNode;
}

export const MainLayout: React.FC<MainLayoutProps> = ({ children }) => {
  const styles = useStyles();
  const { theme } = useAppStore();
  const [sidebarOpen, setSidebarOpen] = useState(true);

  const currentTheme = theme === 'dark' ? webDarkTheme : webLightTheme;

  return (
    <FluentProvider theme={currentTheme}>
      <div className={styles.root}>
        <Header onToggleSidebar={() => setSidebarOpen(!sidebarOpen)} />
        <div className={styles.main}>
          {sidebarOpen && (
            <div className={styles.sidebar}>
              <Sidebar />
            </div>
          )}
          <div className={styles.content}>
            <Navigation />
            <main style={{ flex: 1, overflow: 'auto', padding: '16px' }}>
              {children}
            </main>
          </div>
        </div>
      </div>
    </FluentProvider>
  );
};