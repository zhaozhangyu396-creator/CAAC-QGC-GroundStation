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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlyDataPanel.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacMissionDetails.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacMapSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacRouteLegLabels.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacTemplateEditor.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacGlobalSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacPanelDragHandle.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacCoordinateField.qml
)
qt_add_qml_module(
    CaacPlanModule
    URI CAAC.Plan
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES src/PlanView/CaacRouteEditor.qml
              src/PlanView/CaacWaypointDialog.qml
              src/PlanView/CaacGeometryDialog.qml
              src/PlanView/CaacRelativeEditor.qml
              src/Flight/CaacFlyDataPanel.qml
              src/Flight/CaacMissionDetails.qml
              src/Flight/CaacMapSettings.qml
              src/PlanView/CaacRouteLegLabels.qml
              src/PlanView/CaacTemplateEditor.qml
              src/PlanView/CaacGlobalSettings.qml
              src/PlanView/CaacPanelDragHandle.qml
              src/PlanView/CaacCoordinateField.qml
    NO_PLUGIN
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/FlyViewCustomLayer.qml
                            PROPERTIES QT_RESOURCE_ALIAS "QGroundControl/FlyView/FlyViewCustomLayer.qml"
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_flyview_override
    PREFIX "/Custom/qml"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/FlyViewCustomLayer.qml
)

set(CUSTOM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacCorePlugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacCorePlugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteEditorController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteEditorController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteGeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteGeometry.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteTemplates.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Route/CaacRouteTemplates.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacDirectionalDistance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacDirectionalDistance.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightDistance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightDistance.cc
    PARENT_SCOPE
)
set(CUSTOM_LIBRARIES
    CaacPlanModule
    PARENT_SCOPE
)
set(CUSTOM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/Route ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight
    PARENT_SCOPE
)
set(CUSTOM_DEFINITIONS
    QGC_CUSTOM_BUILD CUSTOMHEADER="CaacCorePlugin.h" CUSTOMCLASS=CaacCorePlugin
    PARENT_SCOPE
)
