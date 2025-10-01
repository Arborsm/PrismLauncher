import React from 'react';
import {
  Breadcrumb,
  BreadcrumbItem,
  BreadcrumbButton,
  makeStyles,
  tokens,
} from '@fluentui/react-components';
import { useLocation, useNavigate } from 'react-router-dom';

const useStyles = makeStyles({
  navigation: {
    padding: '8px 16px',
    borderBottom: `1px solid ${tokens.colorNeutralStroke1}`,
    backgroundColor: tokens.colorNeutralBackground1,
  },
});

export const Navigation: React.FC = () => {
  const styles = useStyles();
  const location = useLocation();
  const navigate = useNavigate();

  const pathSegments = location.pathname.split('/').filter(Boolean);
  
  const breadcrumbItems = [
    { label: 'Home', path: '/' },
    ...pathSegments.map((segment, index) => ({
      label: segment.charAt(0).toUpperCase() + segment.slice(1),
      path: '/' + pathSegments.slice(0, index + 1).join('/'),
    })),
  ];

  const handleBreadcrumbClick = (path: string) => {
    navigate(path);
  };

  return (
    <nav className={styles.navigation}>
      <Breadcrumb>
        {breadcrumbItems.map((item, index) => (
          <BreadcrumbItem key={item.path}>
            <BreadcrumbButton
              onClick={() => handleBreadcrumbClick(item.path)}
              current={index === breadcrumbItems.length - 1}
            >
              {item.label}
            </BreadcrumbButton>
          </BreadcrumbItem>
        ))}
      </Breadcrumb>
    </nav>
  );
};