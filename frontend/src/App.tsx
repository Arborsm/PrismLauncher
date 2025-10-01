import React, { useEffect } from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';
import { MainLayout } from '@/components/Layout/MainLayout';
import { HomePage } from '@/pages/HomePage';
import { InstanceList } from '@/components/Instance/InstanceList';
import { useAppStore } from '@/stores/appStore';

function App() {
  const { initialize } = useAppStore();

  useEffect(() => {
    initialize();
  }, [initialize]);

  return (
    <Router>
      <MainLayout>
        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/instances" element={<InstanceList />} />
          <Route path="/accounts" element={<div>Accounts Page</div>} />
          <Route path="/mods" element={<div>Mods Page</div>} />
          <Route path="/settings" element={<div>Settings Page</div>} />
          <Route path="/about" element={<div>About Page</div>} />
        </Routes>
      </MainLayout>
      <Toaster
        position="top-right"
        toastOptions={{
          duration: 4000,
          style: {
            background: '#363636',
            color: '#fff',
          },
        }}
      />
    </Router>
  );
}

export default App;