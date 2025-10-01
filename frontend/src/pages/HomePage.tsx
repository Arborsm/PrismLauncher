import React, { useEffect } from 'react';
import {
  Card,
  CardHeader,
  CardPreview,
  CardFooter,
  Button,
  Text,
  makeStyles,
  tokens,
  Spinner,
  MessageBar,
  MessageBarBody,
} from '@fluentui/react-components';
import {
  GamepadRegular,
  PersonRegular,
  DownloadRegular,
  SettingsRegular,
  PlayRegular,
  AddRegular,
  ErrorCircleRegular,
} from '@fluentui/react-icons';
import { useNavigate } from 'react-router-dom';
import { useAppStore } from '@/stores/appStore';
import { useInstanceStore } from '@/stores/instanceStore';

const useStyles = makeStyles({
  container: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
    padding: '24px',
  },
  header: {
    marginBottom: '32px',
  },
  title: {
    fontSize: '32px',
    fontWeight: tokens.fontWeightSemibold,
    color: tokens.colorNeutralForeground1,
    marginBottom: '8px',
  },
  subtitle: {
    fontSize: '16px',
    color: tokens.colorNeutralForeground2,
  },
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))',
    gap: '24px',
    marginBottom: '32px',
  },
  card: {
    height: '200px',
  },
  preview: {
    height: '120px',
    backgroundColor: tokens.colorNeutralBackground2,
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    fontSize: '48px',
    color: tokens.colorNeutralForeground3,
  },
  footer: {
    padding: '16px',
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  stats: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
    gap: '16px',
  },
  statCard: {
    padding: '16px',
    backgroundColor: tokens.colorNeutralBackground1,
    border: `1px solid ${tokens.colorNeutralStroke1}`,
    borderRadius: tokens.borderRadiusMedium,
  },
  statValue: {
    fontSize: '24px',
    fontWeight: tokens.fontWeightSemibold,
    color: tokens.colorNeutralForeground1,
  },
  statLabel: {
    fontSize: '14px',
    color: tokens.colorNeutralForeground2,
    marginTop: '4px',
  },
  loading: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: '200px',
  },
  error: {
    margin: '16px 0',
  },
});

export const HomePage: React.FC = () => {
  const styles = useStyles();
  const navigate = useNavigate();
  const { systemInfo, loading: appLoading, error: appError } = useAppStore();
  const { instances, loading: instancesLoading, error: instancesError } = useInstanceStore();

  useEffect(() => {
    // Initialize app if not already done
    if (!systemInfo) {
      useAppStore.getState().initialize();
    }
  }, [systemInfo]);

  const handleNavigate = (path: string) => {
    navigate(path);
  };

  const handleQuickLaunch = () => {
    if (instances.length > 0) {
      const firstInstance = instances[0];
      useInstanceStore.getState().launchInstance(firstInstance.id);
    }
  };

  const runningInstances = instances.filter(instance => instance.isRunning).length;
  const totalInstances = instances.length;

  if (appLoading || instancesLoading) {
    return (
      <div className={styles.container}>
        <div className={styles.loading}>
          <Spinner size="large" />
        </div>
      </div>
    );
  }

  return (
    <div className={styles.container}>
      <div className={styles.header}>
        <Text className={styles.title}>Welcome to Prism Launcher</Text>
        <Text className={styles.subtitle}>
          Manage your Minecraft instances with ease
        </Text>
      </div>

      {(appError || instancesError) && (
        <MessageBar intent="error" className={styles.error}>
          <MessageBarBody>
            <ErrorCircleRegular />
            {appError || instancesError}
          </MessageBarBody>
        </MessageBar>
      )}

      <div className={styles.grid}>
        <Card className={styles.card}>
          <CardPreview className={styles.preview}>
            <GamepadRegular />
          </CardPreview>
          <CardHeader
            header={<Text weight="semibold">Instances</Text>}
            description={<Text size={200}>Manage your Minecraft instances</Text>}
          />
          <CardFooter className={styles.footer}>
            <Text size={200}>{totalInstances} instances</Text>
            <Button
              appearance="primary"
              icon={<AddRegular />}
              onClick={() => handleNavigate('/instances')}
            >
              Manage
            </Button>
          </CardFooter>
        </Card>

        <Card className={styles.card}>
          <CardPreview className={styles.preview}>
            <PersonRegular />
          </CardPreview>
          <CardHeader
            header={<Text weight="semibold">Accounts</Text>}
            description={<Text size={200}>Manage your Minecraft accounts</Text>}
          />
          <CardFooter className={styles.footer}>
            <Text size={200}>0 accounts</Text>
            <Button
              appearance="primary"
              icon={<AddRegular />}
              onClick={() => handleNavigate('/accounts')}
            >
              Manage
            </Button>
          </CardFooter>
        </Card>

        <Card className={styles.card}>
          <CardPreview className={styles.preview}>
            <DownloadRegular />
          </CardPreview>
          <CardHeader
            header={<Text weight="semibold">Mods</Text>}
            description={<Text size={200}>Browse and install mods</Text>}
          />
          <CardFooter className={styles.footer}>
            <Text size={200}>0 mods</Text>
            <Button
              appearance="primary"
              icon={<AddRegular />}
              onClick={() => handleNavigate('/mods')}
            >
              Browse
            </Button>
          </CardFooter>
        </Card>

        <Card className={styles.card}>
          <CardPreview className={styles.preview}>
            <SettingsRegular />
          </CardPreview>
          <CardHeader
            header={<Text weight="semibold">Settings</Text>}
            description={<Text size={200}>Configure launcher settings</Text>}
          />
          <CardFooter className={styles.footer}>
            <Button
              appearance="primary"
              icon={<SettingsRegular />}
              onClick={() => handleNavigate('/settings')}
            >
              Configure
            </Button>
          </CardFooter>
        </Card>
      </div>

      <div className={styles.stats}>
        <div className={styles.statCard}>
          <Text className={styles.statValue}>{totalInstances}</Text>
          <Text className={styles.statLabel}>Total Instances</Text>
        </div>
        <div className={styles.statCard}>
          <Text className={styles.statValue}>{runningInstances}</Text>
          <Text className={styles.statLabel}>Running Instances</Text>
        </div>
        <div className={styles.statCard}>
          <Text className={styles.statValue}>
            {systemInfo?.launcherVersion || 'Unknown'}
          </Text>
          <Text className={styles.statLabel}>Launcher Version</Text>
        </div>
        <div className={styles.statCard}>
          <Text className={styles.statValue}>
            {systemInfo?.javaVersion || 'Unknown'}
          </Text>
          <Text className={styles.statLabel}>Java Version</Text>
        </div>
      </div>

      {totalInstances > 0 && (
        <div style={{ marginTop: '32px', textAlign: 'center' }}>
          <Button
            appearance="primary"
            size="large"
            icon={<PlayRegular />}
            onClick={handleQuickLaunch}
          >
            Quick Launch
          </Button>
        </div>
      )}
    </div>
  );
};