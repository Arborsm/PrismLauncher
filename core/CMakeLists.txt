cmake_minimum_required(VERSION 3.22)

project(PrismCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core sources without Qt dependencies
set(CORE_SOURCES
    # Base infrastructure
    BaseInstance.h
    BaseInstance.cpp
    BaseVersion.h
    BaseVersionList.h
    BaseVersionList.cpp
    InstanceList.h
    InstanceList.cpp
    InstanceTask.h
    InstanceTask.cpp
    LoggedProcess.h
    LoggedProcess.cpp
    MessageLevel.h
    MessageLevel.cpp
    
    # File operations
    FileSystem.h
    FileSystem.cpp
    MMCZip.h
    MMCZip.cpp
    Untar.h
    Untar.cpp
    GZip.h
    GZip.cpp
    
    # Utilities
    StringUtils.h
    StringUtils.cpp
    Version.h
    Version.cpp
    Json.h
    Json.cpp
    Exception.h
    DefaultVariable.h
    Filter.h
    SeparatorPrefixTree.h
    RWStorage.h
    Usable.h
    PSaveFile.h
    RuntimeContext.h
    QVariantUtils.h
    
    # Time
    MMCTime.h
    MMCTime.cpp
    
    # Compression
    ExponentialSeries.h
    
    # Network (without Qt)
    net/NetJob.h
    net/NetJob.cpp
    net/Download.h
    net/Download.cpp
    net/Upload.h
    net/Upload.cpp
    net/Sink.h
    net/Validator.h
    net/ChecksumValidator.h
    net/FileSink.h
    net/FileSink.cpp
    net/ByteArraySink.h
    net/MetaCacheSink.h
    net/MetaCacheSink.cpp
    net/HttpMetaCache.h
    net/HttpMetaCache.cpp
    net/Logging.h
    net/Logging.cpp
    net/NetUtils.h
    net/NetRequest.h
    net/NetRequest.cpp
    net/HeaderProxy.h
    net/RawHeaderProxy.h
    net/ApiHeaderProxy.h
    net/ApiDownload.h
    net/ApiDownload.cpp
    net/ApiUpload.h
    net/ApiUpload.cpp
    net/PasteUpload.h
    net/PasteUpload.cpp
    
    # Tasks
    tasks/Task.h
    tasks/Task.cpp
    tasks/ConcurrentTask.h
    tasks/ConcurrentTask.cpp
    tasks/SequentialTask.h
    tasks/SequentialTask.cpp
    tasks/MultipleOptionsTask.h
    tasks/MultipleOptionsTask.cpp
    
    # Settings
    settings/SettingsObject.h
    settings/SettingsObject.cpp
    settings/Setting.h
    settings/Setting.cpp
    settings/INIFile.h
    settings/INIFile.cpp
    settings/INISettingsObject.h
    settings/INISettingsObject.cpp
    settings/OverrideSetting.h
    settings/OverrideSetting.cpp
    settings/PassthroughSetting.h
    settings/PassthroughSetting.cpp
    
    # Java
    java/JavaChecker.h
    java/JavaChecker.cpp
    java/JavaInstall.h
    java/JavaInstall.cpp
    java/JavaInstallList.h
    java/JavaInstallList.cpp
    java/JavaUtils.h
    java/JavaUtils.cpp
    java/JavaVersion.h
    java/JavaVersion.cpp
    java/JavaMetadata.h
    java/JavaMetadata.cpp
    
    # Minecraft
    minecraft/MinecraftInstance.h
    minecraft/MinecraftInstance.cpp
    minecraft/LaunchProfile.h
    minecraft/LaunchProfile.cpp
    minecraft/Component.h
    minecraft/Component.cpp
    minecraft/PackProfile.h
    minecraft/PackProfile.cpp
    minecraft/ComponentUpdateTask.h
    minecraft/ComponentUpdateTask.cpp
    minecraft/MinecraftLoadAndCheck.h
    minecraft/MinecraftLoadAndCheck.cpp
    minecraft/MojangVersionFormat.h
    minecraft/MojangVersionFormat.cpp
    minecraft/Rule.h
    minecraft/Rule.cpp
    minecraft/OneSixVersionFormat.h
    minecraft/OneSixVersionFormat.cpp
    minecraft/ParseUtils.h
    minecraft/ParseUtils.cpp
    minecraft/ProfileUtils.h
    minecraft/ProfileUtils.cpp
    minecraft/ShortcutUtils.h
    minecraft/ShortcutUtils.cpp
    minecraft/Library.h
    minecraft/Library.cpp
    minecraft/MojangDownloadInfo.h
    minecraft/VanillaInstanceCreationTask.h
    minecraft/VanillaInstanceCreationTask.cpp
    minecraft/VersionFile.h
    minecraft/VersionFile.cpp
    minecraft/VersionFilterData.h
    minecraft/VersionFilterData.cpp
    minecraft/World.h
    minecraft/World.cpp
    minecraft/WorldList.h
    minecraft/WorldList.cpp
    minecraft/GradleSpecifier.h
    minecraft/Agent.h
    
    # Minecraft Auth
    minecraft/auth/AccountData.h
    minecraft/auth/AccountData.cpp
    minecraft/auth/AccountList.h
    minecraft/auth/AccountList.cpp
    minecraft/auth/AuthSession.h
    minecraft/auth/AuthSession.cpp
    minecraft/auth/AuthStep.h
    minecraft/auth/MinecraftAccount.h
    minecraft/auth/MinecraftAccount.cpp
    minecraft/auth/Parsers.h
    minecraft/auth/Parsers.cpp
    minecraft/auth/AuthFlow.h
    minecraft/auth/AuthFlow.cpp
    
    # Minecraft Launch
    minecraft/launch/LaunchStep.h
    minecraft/launch/LaunchTask.h
    minecraft/launch/LaunchTask.cpp
    minecraft/launch/LogModel.h
    minecraft/launch/LogModel.cpp
    minecraft/launch/TaskStepWrapper.h
    minecraft/launch/TaskStepWrapper.cpp
    minecraft/launch/MinecraftTarget.h
    minecraft/launch/MinecraftTarget.cpp
    
    # Mod platform
    modplatform/ModIndex.h
    modplatform/ModIndex.cpp
    modplatform/ResourceType.h
    modplatform/ResourceType.cpp
    modplatform/ResourceAPI.h
    modplatform/ResourceAPI.cpp
    modplatform/EnsureMetadataTask.h
    modplatform/EnsureMetadataTask.cpp
    modplatform/CheckUpdateTask.h
    
    # Meta
    meta/JsonFormat.h
    meta/JsonFormat.cpp
    meta/BaseEntity.h
    meta/BaseEntity.cpp
    meta/VersionList.h
    meta/VersionList.cpp
    meta/Version.h
    meta/Version.cpp
    meta/Index.h
    meta/Index.cpp
    
    # System info
    SysInfo.h
    SysInfo.cpp
    
    # Command line
    Commandline.h
    Commandline.cpp
)

# Create core library
add_library(PrismCore STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(PrismCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../launcher
)

# Link libraries
target_link_libraries(PrismCore
    systeminfo
    nbt++
    ${ZLIB_LIBRARIES}
    qdcss
    BuildConfig
    qrcodegenerator
)

# Add tomlplusplus
if(TARGET PkgConfig::tomlplusplus)
    target_link_libraries(PrismCore PkgConfig::tomlplusplus)
else()
    target_link_libraries(PrismCore tomlplusplus::tomlplusplus)
endif()

# Compiler definitions
target_compile_definitions(PrismCore PUBLIC 
    PRISM_CORE_BUILD
    PRISM_NO_QT
)

# Install
install(TARGETS PrismCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/PrismCore
    FILES_MATCHING PATTERN "*.h"
)