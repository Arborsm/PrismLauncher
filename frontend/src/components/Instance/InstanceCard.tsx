import React from 'react';
import {
  Card,
  CardHeader,
  CardPreview,
  CardFooter,
  Button,
  Text,
  Badge,
  makeStyles,
  tokens,
  Menu,
  MenuTrigger,
  MenuPopover,
  MenuList,
  MenuItem,
  MenuDivider,
} from '@fluentui/react-components';
import {
  PlayRegular,
  StopRegular,
  MoreHorizontalRegular,
  EditRegular,
  DeleteRegular,
  CopyRegular,
  FolderRegular,
} from '@fluentui/react-icons';
import { InstanceInfo } from '@/types';

const useStyles = makeStyles({
  card: {
    width: '100%',
    maxWidth: '300px',
    margin: '8px',
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
  header: {
    padding: '12px',
  },
  footer: {
    padding: '12px',
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  statusBadge: {
    marginLeft: '8px',
  },
  actions: {
    display: 'flex',
    gap: '8px',
  },
});

interface InstanceCardProps {
  instance: InstanceInfo;
  onLaunch: (id: string) => void;
  onStop: (id: string) => void;
  onEdit: (id: string) => void;
  onDelete: (id: string) => void;
  onCopy: (id: string) => void;
  onOpenFolder: (id: string) => void;
}

export const InstanceCard: React.FC<InstanceCardProps> = ({
  instance,
  onLaunch,
  onStop,
  onEdit,
  onDelete,
  onCopy,
  onOpenFolder,
}) => {
  const styles = useStyles();

  const handleLaunch = () => {
    onLaunch(instance.id);
  };

  const handleStop = () => {
    onStop(instance.id);
  };

  const handleEdit = () => {
    onEdit(instance.id);
  };

  const handleDelete = () => {
    onDelete(instance.id);
  };

  const handleCopy = () => {
    onCopy(instance.id);
  };

  const handleOpenFolder = () => {
    onOpenFolder(instance.id);
  };

  return (
    <Card className={styles.card}>
      <CardPreview className={styles.preview}>
        <GamepadRegular />
      </CardPreview>
      
      <CardHeader
        className={styles.header}
        header={
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <Text weight="semibold" size={400}>
              {instance.name}
            </Text>
            <Badge
              appearance={instance.isRunning ? 'filled' : 'outline'}
              color={instance.isRunning ? 'success' : 'neutral'}
              className={styles.statusBadge}
            >
              {instance.isRunning ? 'Running' : 'Stopped'}
            </Badge>
          </div>
        }
        description={
          <Text size={200} style={{ color: tokens.colorNeutralForeground2 }}>
            Version: {instance.version}
          </Text>
        }
      />
      
      <CardFooter className={styles.footer}>
        <div className={styles.actions}>
          {instance.isRunning ? (
            <Button
              appearance="primary"
              icon={<StopRegular />}
              onClick={handleStop}
            >
              Stop
            </Button>
          ) : (
            <Button
              appearance="primary"
              icon={<PlayRegular />}
              onClick={handleLaunch}
            >
              Launch
            </Button>
          )}
          
          <Menu>
            <MenuTrigger>
              <Button appearance="subtle" icon={<MoreHorizontalRegular />} />
            </MenuTrigger>
            <MenuPopover>
              <MenuList>
                <MenuItem icon={<EditRegular />} onClick={handleEdit}>
                  Edit
                </MenuItem>
                <MenuItem icon={<CopyRegular />} onClick={handleCopy}>
                  Copy
                </MenuItem>
                <MenuItem icon={<FolderRegular />} onClick={handleOpenFolder}>
                  Open Folder
                </MenuItem>
                <MenuDivider />
                <MenuItem icon={<DeleteRegular />} onClick={handleDelete}>
                  Delete
                </MenuItem>
              </MenuList>
            </MenuPopover>
          </Menu>
        </div>
      </CardFooter>
    </Card>
  );
};