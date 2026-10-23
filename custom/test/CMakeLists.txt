target_sources(
    ${CMAKE_PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacActivationControllerTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacActivationUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacStartupUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacMissionUpdateControllerTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacWaypointSnapUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacLegSpeedUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFlightDistanceTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFlightDataUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacPlanningToolsTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacSimulationEngineTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacSimulationUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFigureEightControllerTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFigureEightFenceControllerTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFigureEightFenceGeometryTest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFigureEightUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacPlanningInteractionUITest.cc
)
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/CaacActivationControllerTest.cc ${CMAKE_CURRENT_SOURCE_DIR}/CaacActivationUITest.cc
    TARGET_DIRECTORY ${CMAKE_PROJECT_NAME} PROPERTIES INCLUDE_DIRECTORIES
                                                      "$<TARGET_PROPERTY:CaacEd25519,INTERFACE_INCLUDE_DIRECTORIES>"
)
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_add_executable(CaacFocusTestWindow WIN32 CaacFocusTestWindow.cc)
    target_link_libraries(CaacFocusTestWindow PRIVATE Qt6::Gui Qt6::Network)
    if(WIN32)
        target_link_libraries(CaacFocusTestWindow PRIVATE user32)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(CAAC_FOCUS_XCB REQUIRED IMPORTED_TARGET xcb)
        target_link_libraries(CaacFocusTestWindow PRIVATE PkgConfig::CAAC_FOCUS_XCB)
    endif()
    add_dependencies(${CMAKE_PROJECT_NAME} CaacFocusTestWindow)
endif()
add_qgc_test(CaacMissionUpdateControllerTest LABELS Unit MissionManager)
add_qgc_test(CaacActivationControllerTest LABELS Unit Utilities)
add_qgc_test(
    CaacActivationUITest
    LABELS
    Integration
    Utilities
    NoSanitizer
    TIMEOUT
    60
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
)
if(WIN32)
    set_property(
        TEST CaacActivationUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()
add_qgc_test(
    CaacStartupUITest
    LABELS
    Integration
    Utilities
    NoSanitizer
    TIMEOUT
    60
)
if(WIN32)
    set_property(
        TEST CaacStartupUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()
add_qgc_test(CaacFlightDistanceTest LABELS Unit Vehicle)
add_qgc_test(CaacRouteEditorTest LABELS Unit MissionManager)
add_qgc_test(CaacPlanningToolsTest LABELS Unit MissionManager)
add_qgc_test(CaacRouteTemplateGeometryTest LABELS Unit MissionManager)
add_qgc_test(
    CaacChineseUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    120
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
)
if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dbghelp)
    # The Windows offscreen plugin has no system font database; use the native platform for real UI geometry.
    set(_caac_ui_env ENV_MODIFICATION "QT_QPA_PLATFORM=set:windows")
    set_property(
        TEST CaacChineseUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()
add_qgc_test(
    CaacRouteEditorUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    120
    ${_caac_ui_env}
)
add_qgc_test(
    CaacWaypointSnapUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    120
    ${_caac_ui_env}
)
add_qgc_test(
    CaacLegSpeedUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    120
    ${_caac_ui_env}
)
add_qgc_test(
    CaacFlightDataUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    180
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
)
if(WIN32)
    set_property(
        TEST CaacFlightDataUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()

add_qgc_test(CaacSimulationEngineTest LABELS Unit MissionManager)
add_qgc_test(CaacFigureEightControllerTest LABELS Unit Vehicle)
add_qgc_test(CaacFigureEightFenceControllerTest LABELS Unit Vehicle)
add_qgc_test(CaacFigureEightFenceGeometryTest LABELS Unit MissionManager)
add_qgc_test(
    CaacFigureEightUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    120
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    ${_caac_ui_env}
)
add_qgc_test(
    CaacMissionUpdateUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    120
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    "QGC_TEST_REALTIME_PLAN_TIMEOUTS=set:1"
)
if(WIN32)
    set_property(
        TEST CaacMissionUpdateUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()
add_qgc_test(
    CaacSimulationUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    180
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    "QGC_TEST_REALTIME_PLAN_TIMEOUTS=set:1"
)
add_qgc_test(
    CaacSimulationFlowUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    180
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    "QGC_TEST_REALTIME_PLAN_TIMEOUTS=set:1"
)
if(WIN32)
    set_property(
        TEST CaacSimulationUITest CaacSimulationFlowUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()

add_qgc_test(
    CaacDefaultHomeUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    "QGC_TEST_REALTIME_PLAN_TIMEOUTS=set:1"
)
if(WIN32)
    set_property(
        TEST CaacDefaultHomeUITest
        APPEND
        PROPERTY ENVIRONMENT_MODIFICATION "QT_QPA_PLATFORM=set:windows"
    )
endif()

add_qgc_test(
    CaacPlanningInteractionUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    300
    ENV_MODIFICATION
    "QGC_TEST_LOCALE=set:zh_CN"
    ${_caac_ui_env}
)
