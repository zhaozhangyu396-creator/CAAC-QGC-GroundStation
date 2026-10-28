set(QGC_CAAC_BUILD
    TRUE
    PARENT_SCOPE
)

CPMAddPackage(
    NAME caac_monocypher
    URL https://codeload.github.com/LoupVaillant/Monocypher/tar.gz/refs/tags/4.0.2
    URL_HASH SHA256=bc1ca30b1b2654e4e7daf2492c0d204200e55137f23fda6b7142fd7d523bd6b4
    DOWNLOAD_ONLY YES
)
add_library(CaacEd25519 STATIC ${caac_monocypher_SOURCE_DIR}/src/monocypher.c
                               ${caac_monocypher_SOURCE_DIR}/src/optional/monocypher-ed25519.c
)
target_include_directories(CaacEd25519 SYSTEM PUBLIC ${caac_monocypher_SOURCE_DIR}/src
                                                     ${caac_monocypher_SOURCE_DIR}/src/optional
)
set_target_properties(CaacEd25519 PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(CaacActivationCrypto STATIC src/Activation/CaacActivationCodec.cc src/Activation/CaacActivationCodec.h)
target_include_directories(CaacActivationCrypto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Activation)
target_link_libraries(CaacActivationCrypto PRIVATE Qt6::Core CaacEd25519)
set_target_properties(CaacActivationCrypto PROPERTIES POSITION_INDEPENDENT_CODE ON)

# The issuer is deliberately excluded from application builds and installer contents.
add_executable(CaacLicenseTool EXCLUDE_FROM_ALL tools/CaacLicenseTool.cc)
target_include_directories(CaacLicenseTool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Activation)
target_link_libraries(CaacLicenseTool PRIVATE CaacActivationCrypto Qt6::Core CaacEd25519)

set(CAAC_ACTIVATION_PUBLIC_KEY_HEX "")
set(_caac_public_key_file ${CMAKE_CURRENT_SOURCE_DIR}/activation/public-key.hex)
set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS ${_caac_public_key_file}
)
if(EXISTS ${_caac_public_key_file})
    file(READ ${_caac_public_key_file} CAAC_ACTIVATION_PUBLIC_KEY_HEX)
    string(STRIP "${CAAC_ACTIVATION_PUBLIC_KEY_HEX}" CAAC_ACTIVATION_PUBLIC_KEY_HEX)
    string(LENGTH "${CAAC_ACTIVATION_PUBLIC_KEY_HEX}" _caac_public_key_length)
    if(NOT _caac_public_key_length EQUAL 64 OR NOT CAAC_ACTIVATION_PUBLIC_KEY_HEX MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR "CAAC activation public key must contain exactly 64 hexadecimal characters")
    endif()
else()
    message(STATUS "CAAC activation public key is not configured; normal startup will stay locked")
endif()
configure_file(src/Activation/CaacActivationPublicKey.h.in CaacActivationPublicKey.h @ONLY)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Activation/CaacActivationController.cc TARGET_DIRECTORY
                            ${CMAKE_PROJECT_NAME} PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacWaypointGuides.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacHomeRelationsDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacTransformSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacUnifiedFlightLayer.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightSpeedDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightDashboard.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightTelemetryBar.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacTemplateEditor.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacSettingsSection.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacGlobalSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacPanelDragHandle.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacCoordinateField.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacMeasureDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacMeasureMap.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacUnitConverter.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacCalculator.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacCalculatorMath.js
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacPreferences.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacSettingsMenu.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacConnectionSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacRIDSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacSafetySettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacFailsafeEnumTranslations.js
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacBatteryProtection.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacGeoFenceSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacUOMSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacAttitudeInstrument.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationEmergencyDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationJoystickSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightIllustration.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightMap.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacMissionActionDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacSimulationSettings.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacSimulationDialog.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacWaypointOverlap.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacWaypointPopup.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacWaypointMenu.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlanView/CaacDrawingAids.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Activation/CaacActivationWindow.qml
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
              src/PlanView/CaacWaypointGuides.qml
              src/PlanView/CaacHomeRelationsDialog.qml
              src/PlanView/CaacTransformSettings.qml
              src/Flight/CaacUnifiedFlightLayer.qml
              src/Flight/CaacFlightSpeedDialog.qml
              src/Flight/CaacFlightDashboard.qml
              src/Flight/CaacFlightTelemetryBar.qml
              src/PlanView/CaacTemplateEditor.qml
              src/PlanView/CaacSettingsSection.qml
              src/PlanView/CaacGlobalSettings.qml
              src/PlanView/CaacPanelDragHandle.qml
              src/PlanView/CaacCoordinateField.qml
              src/PlanView/CaacMeasureDialog.qml
              src/PlanView/CaacMeasureMap.qml
              src/PlanView/CaacUnitConverter.qml
              src/PlanView/CaacCalculator.qml
              src/PlanView/CaacCalculatorMath.js
              src/PlanView/CaacPreferences.qml
              src/PlanView/CaacSettingsMenu.qml
              src/PlanView/CaacConnectionSettings.qml
              src/PlanView/CaacRIDSettings.qml
              src/PlanView/CaacSafetySettings.qml
              src/PlanView/CaacFailsafeEnumTranslations.js
              src/PlanView/CaacBatteryProtection.qml
              src/PlanView/CaacGeoFenceSettings.qml
              src/PlanView/CaacUOMSettings.qml
              src/Flight/CaacAttitudeInstrument.qml
              src/Flight/CaacSimulationEmergencyDialog.qml
              src/Flight/CaacSimulationJoystickSettings.qml
              src/Flight/CaacFigureEightDialog.qml
              src/Flight/CaacFigureEightIllustration.qml
              src/Flight/CaacFigureEightMap.qml
              src/Flight/CaacMissionActionDialog.qml
              src/PlanView/CaacSimulationSettings.qml
              src/PlanView/CaacSimulationDialog.qml
              src/PlanView/CaacWaypointOverlap.qml
              src/PlanView/CaacWaypointPopup.qml
              src/PlanView/CaacWaypointMenu.qml
              src/PlanView/CaacDrawingAids.qml
              src/Activation/CaacActivationWindow.qml
    NO_PLUGIN
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/FlyViewCustomLayer.qml
                            PROPERTIES QT_RESOURCE_ALIAS "QGroundControl/FlyView/FlyViewCustomLayer.qml"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightActionList.qml
                            PROPERTIES QT_RESOURCE_ALIAS "QGroundControl/FlyView/CaacFlightActionList.qml"
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_flyview_override
    PREFIX "/Custom/qml"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/FlyViewCustomLayer.qml
          ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFlightActionList.qml
)

set(CUSTOM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacActivationController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Activation/CaacActivationController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CaacStartupController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Startup/CaacStartupController.cc
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationEngine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationEngine.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacMissionUpdateController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacMissionUpdateController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacSimulationController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightFenceController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightFenceController.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightFenceGeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacFigureEightFenceGeometry.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacExamAudio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight/CaacExamAudio.cc
    PARENT_SCOPE
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.svg PROPERTIES QT_RESOURCE_ALIAS
                                                                                           "company-logo.svg"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.ico PROPERTIES QT_RESOURCE_ALIAS
                                                                                           "company-logo.ico"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.png PROPERTIES QT_RESOURCE_ALIAS
                                                                                           "company-logo.png"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/startup-background.png
                            PROPERTIES QT_RESOURCE_ALIAS "startup-background.png"
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_company_brand
    PREFIX "/CAAC/Brand"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.svg ${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.ico
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/company-logo.png ${CMAKE_CURRENT_SOURCE_DIR}/assets/startup-background.png
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_startup
    PREFIX "/CAAC/Startup"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/src/Startup"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Startup/CaacStartupWindow.qml
)
set(CUSTOM_LIBRARIES
    CaacPlanModule CaacActivationCrypto
    PARENT_SCOPE
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_mission_images
    PREFIX "/qmlimages"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/MapPlannedLanding.svg
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_calibration_images
    PREFIX "/CAAC/Calibration"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-01.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-02.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-03.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-04.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-05.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-06.png
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-01-rotation.gif
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-02-rotation.gif
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-03-rotation.gif
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-04-rotation.gif
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-05-rotation.gif
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/calibration/pose-06-rotation.gif
)
qt_add_resources(
    ${CMAKE_PROJECT_NAME} caac_exam_audio
    PREFIX "/CAAC/ExamAudio"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/manifest.json
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/MeloTTS-LICENSE.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/positioning.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/retry.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/failed.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/ready.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/spin.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/spin_reverse.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/spin_completed.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/spin_timeout.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/figure_eight.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/area.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/speed_high.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/speed_low.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/time_up.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/heading.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/altitude.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/completed.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/stopped.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/telemetry.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/wind.wav
          ${CMAKE_CURRENT_SOURCE_DIR}/assets/exam-audio/spin_too_fast.wav
)
set(CUSTOM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/Route ${CMAKE_CURRENT_SOURCE_DIR}/src/Flight
    PARENT_SCOPE
)
set(CUSTOM_DEFINITIONS
    QGC_CUSTOM_BUILD QGC_CAAC_SIMULATION CUSTOMHEADER="CaacCorePlugin.h" CUSTOMCLASS=CaacCorePlugin
    PARENT_SCOPE
)
