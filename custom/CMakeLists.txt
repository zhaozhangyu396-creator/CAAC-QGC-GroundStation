set(QGC_CAAC_BUILD
    TRUE
    PARENT_SCOPE
)
qt_add_library(CaacPlanModule STATIC)
qgc_configure_target(CaacPlanModule)

qgc_set_qt_resource_alias(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacRouteEditor.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacWaypointDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacGeometryDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacRelativeEditor.qml
)
qt_add_qml_module(
    CaacPlanModule
    URI CAAC.Plan
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES src/PlanView/CaacRouteEditor.qml src/PlanView/CaacWaypointDialog.qml src/PlanView/CaacGeometryDialog.qml
              src/PlanView/CaacRelativeEditor.qml
    NO_PLUGIN
)

set(CUSTOM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacCorePlugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacCorePlugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteEditorController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteEditorController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteGeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteGeometry.cc
    PARENT_SCOPE
)
set(CUSTOM_LIBRARIES
    CaacPlanModule
    PARENT_SCOPE
)
set(CUSTOM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/Route
    PARENT_SCOPE
)
set(CUSTOM_DEFINITIONS
    QGC_CUSTOM_BUILD CUSTOMHEADER="CaacCorePlugin.h" CUSTOMCLASS=CaacCorePlugin
    PARENT_SCOPE
)
