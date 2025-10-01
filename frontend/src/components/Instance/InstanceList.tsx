import React, { useState, useEffect } from 'react';
import {
  Button,
  Text,
  makeStyles,
  tokens,
  Spinner,
  MessageBar,
  MessageBarBody,
} from '@fluentui/react-components';
import {
  AddRegular,
  RefreshRegular,
  ErrorCircleRegular,
} from '@fluentui/react-icons';
import { InstanceCard } from './InstanceCard';
import { InstanceInfo } from '@/types';
import { useInstanceStore } from '@/stores/instanceStore';

const useStyles = makeStyles({
  container: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
  },
  header: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: '16px',
    padding: '16px 0',
    borderBottom: `1px solid ${tokens.colorNeutralStroke1}`,
  },
  title: {
    fontSize: '24px',
    fontWeight: tokens.fontWeightSemibold,
    color: tokens.colorNeutralForeground1,
  },
  actions: {
    display: 'flex',
    gap: '12px',
  },
  content: {
    flex: 1,
    overflow: 'auto',
  },
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(300px, 1fr))',
    gap: '16px',
    padding: '16px 0',
  },
  empty: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
    height: '200px',
    color: tokens.colorNeutralForeground3,
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

export const InstanceList: React.FC = () => {
  const styles = useStyles();
  const {
    instances,
    loading,
    error,
    fetchInstances,
    launchInstance,
    stopInstance,
    deleteInstance,
  } = useInstanceStore();

  useEffect(() => {
    fetchInstances();
  }, [fetchInstances]);

  const handleRefresh = () => {
    fetchInstances();
  };

  const handleCreate = () => {
    // Navigate to create instance page
    console.log('Create new instance');
  };

  const handleLaunch = (id: string) => {
    launchInstance(id);
  };

  const handleStop = (id: string) => {
    stopInstance(id);
  };

  const handleEdit = (id: string) => {
    // Navigate to edit instance page
    console.log('Edit instance:', id);
  };

  const handleDelete = (id: string) => {
    if (window.confirm('Are you sure you want to delete this instance?')) {
      deleteInstance(id);
    }
  };

  const handleCopy = (id: string) => {
    // Copy instance logic
    console.log('Copy instance:', id);
  };

  const handleOpenFolder = (id: string) => {
    // Open instance folder
    console.log('Open folder for instance:', id);
  };

  if (loading) {
    return (
      <div className={styles.container}>
        <div className={styles.header}>
          <Text className={styles.title}>Instances</Text>
        </div>
        <div className={styles.loading}>
          <Spinner size="large" />
        </div>
      </div>
    );
  }

  return (
    <div className={styles.container}>
      <div className={styles.header}>
        <Text className={styles.title}>Instances</Text>
        <div className={styles.actions}>
          <Button
            appearance="subtle"
            icon={<RefreshRegular />}
            onClick={handleRefresh}
          >
            Refresh
          </Button>
          <Button
            appearance="primary"
            icon={<AddRegular />}
            onClick={handleCreate}
          >
            Create Instance
          </Button>
        </div>
      </div>

      {error && (
        <MessageBar intent="error" className={styles.error}>
          <MessageBarBody>
            <ErrorCircleRegular />
            {error}
          </MessageBarBody>
        </MessageBar>
      )}

      <div className={styles.content}>
        {instances.length === 0 ? (
          <div className={styles.empty}>
            <Text size={400} style={{ marginBottom: '8px' }}>
              No instances found
            </Text>
            <Text size={200} style={{ color: tokens.colorNeutralForeground3 }}>
              Create your first instance to get started
            </Text>
          </div>
        ) : (
          <div className={styles.grid}>
            {instances.map((instance) => (
              <InstanceCard
                key={instance.id}
                instance={instance}
                onLaunch={handleLaunch}
                onStop={handleStop}
                onEdit={handleEdit}
                onDelete={handleDelete}
                onCopy={handleCopy}
                onOpenFolder={handleOpenFolder}
              />
            ))}
          </div>
        )}
      </div>
    </div>
  );
};