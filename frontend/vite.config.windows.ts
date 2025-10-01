import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import electron from 'vite-plugin-electron'

export default defineConfig({
  plugins: [
    react(),
    electron({
      main: {
        entry: 'electron/main.ts',
      },
      preload: {
        input: 'electron/preload.ts',
      },
    }),
  ],
  server: {
    port: 5173,
    host: '0.0.0.0', // 允许外部访问
  },
  build: {
    outDir: 'dist',
    sourcemap: true, // 生成sourcemap用于调试
  },
  // Windows特定优化
  optimizeDeps: {
    include: [
      '@fluentui/react-components',
      '@fluentui/react-icons',
      'react',
      'react-dom',
      'react-router-dom',
      'zustand',
      'axios',
    ],
  },
  // 解决Windows路径问题
  resolve: {
    alias: {
      '@': '/src',
    },
  },
})