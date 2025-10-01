cmake_minimum_required(VERSION 3.20)

project(PrismLauncherWinUI3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Windows App SDK版本
set(WINDOWS_APP_SDK_VERSION "1.4.231008000")

# 查找Windows App SDK
find_package(WindowsAppSDK REQUIRED)

# 设置WinRT目标版本
set(WINRT_TARGET_VERSION "10.0.19041.0")

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bridge
)

# 源文件
set(SOURCES
    src/main.cpp
    src/App.xaml.cpp
    src/MainWindow.xaml.cpp
    src/MainWindow.xaml.h
    src/App.xaml.h
    src/App.idl
    src/MainWindow.idl
    src/bridge/PrismLauncherBridge.h
    src/bridge/PrismLauncherBridge.cpp
    src/bridge/InstanceWrapper.h
    src/bridge/InstanceWrapper.cpp
    src/bridge/InstanceListWrapper.h
    src/bridge/InstanceListWrapper.cpp
    src/core/PrismCore.h
    src/core/PrismCore.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置WinRT属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    WINRT_COMPONENT TRUE
    WINRT_TARGET_VERSION ${WINRT_TARGET_VERSION}
    VS_WINRT_COMPONENT TRUE
    VS_WINRT_REFERENCES "Microsoft.WindowsAppRuntime"
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制资源文件
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/App.xaml 
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.xaml 
     DESTINATION ${CMAKE_BINARY_DIR}/bin)

# 设置启动参数
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_COMMAND_ARGUMENTS ""
)

# 设置Windows App SDK包引用
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_PACKAGE_REFERENCES "Microsoft.WindowsAppRuntime.1.4;Microsoft.WindowsAppRuntime.Bootstrap.1.4"
)

# 设置应用程序清单
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DP_MAINPACKAGE "Microsoft.WindowsAppRuntime"
    VS_DP_MAINPACKAGEVERSION "1.4.231008000"
)

# 设置目标平台版本
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.19041.0"
)

# 设置C++/WinRT属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_WINRT_COMPONENT TRUE
    VS_WINRT_REFERENCES "Microsoft.WindowsAppRuntime"
)

# 添加预处理器定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WINRT_LEAN_AND_MEAN
    WINRT_IMPL
    WINRT_NO_MAKE_DETECTION
)

# 设置链接器标志
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup"
)