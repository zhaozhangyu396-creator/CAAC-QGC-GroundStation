# Keep the upstream vehicle factories, communication and video configuration.
set(QGC_APP_DISPLAY_NAME
    "飞虎无人机地面站系统"
    CACHE STRING "Product display name" FORCE
)
set(QGC_WINDOWS_ICON_PATH
    "${CMAKE_SOURCE_DIR}/custom/assets/company-logo.ico"
    CACHE FILEPATH "Application icon" FORCE
)
set(QGC_WINDOWS_INSTALL_HEADER_PATH
    "${CMAKE_SOURCE_DIR}/custom/assets/company-installer-header.bmp"
    CACHE FILEPATH "Installer header image" FORCE
)
