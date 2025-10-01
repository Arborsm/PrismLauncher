cmake_minimum_required(VERSION 3.16)
project(PrismLauncherIPC VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# IPC源文件
set(IPC_SOURCES
    ipc/Message.cpp
    ipc/MessageBus.cpp
    ipc/Client.cpp
)

# 兼容层源文件
set(COMPAT_SOURCES
    compat/String.cpp
    compat/Process.cpp
    compat/Network.cpp
)

# 核心源文件
set(CORE_SOURCES
    core/Application.cpp
)

# 启动器源文件
set(LAUNCHER_SOURCES
    launcher/LauncherCore.cpp
)

# 主程序源文件
set(MAIN_SOURCES
    main.cpp
)

# 创建静态库
add_library(ipc_compat STATIC
    ${IPC_SOURCES}
    ${COMPAT_SOURCES}
    ${CORE_SOURCES}
    ${LAUNCHER_SOURCES}
)

# 设置库属性
target_include_directories(ipc_compat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ipc_compat PUBLIC PRISM_LAUNCHER_IPC)

# 链接系统库
target_link_libraries(ipc_compat
    pthread
    dl
)

# 创建可执行文件
add_executable(prismlauncher_ipc ${MAIN_SOURCES})
target_link_libraries(prismlauncher_ipc ipc_compat)

# 设置可执行文件属性
set_target_properties(prismlauncher_ipc PROPERTIES
    OUTPUT_NAME "prismlauncher-ipc"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 安装规则
install(TARGETS prismlauncher_ipc
    RUNTIME DESTINATION bin
)

install(TARGETS ipc_compat
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# 安装头文件
install(DIRECTORY ipc/ compat/ core/ launcher/
    DESTINATION include/prismlauncher_ipc
    FILES_MATCHING PATTERN "*.h"
)

# 创建客户端示例
add_executable(ipc_client_example
    examples/client_example.cpp
)
target_link_libraries(ipc_client_example ipc_compat)

# 创建测试
enable_testing()

add_executable(test_string
    tests/test_string.cpp
)
target_link_libraries(test_string ipc_compat)

add_executable(test_message
    tests/test_message.cpp
)
target_link_libraries(test_message ipc_compat)

# 添加测试
add_test(NAME StringTest COMMAND test_string)
add_test(NAME MessageTest COMMAND test_message)

# 打包配置
set(CPACK_PACKAGE_NAME "PrismLauncher-IPC")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PrismLauncher IPC Compatibility Layer")
set(CPACK_PACKAGE_VENDOR "IPC Compat Layer")
set(CPACK_PACKAGE_CONTACT "developer@example.com")

include(CPack)