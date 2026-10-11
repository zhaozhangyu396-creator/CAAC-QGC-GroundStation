target_sources(
    ${CMAKE_PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorTest.cc ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorUITest.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/CaacFlightDistanceTest.cc ${CMAKE_CURRENT_SOURCE_DIR}/CaacFlightDataUITest.cc
)
add_qgc_test(CaacFlightDistanceTest LABELS Unit Vehicle)
add_qgc_test(CaacRouteEditorTest LABELS Unit MissionManager)
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
    CaacFlightDataUITest
    LABELS
    Integration
    Vehicle
    NoSanitizer
    TIMEOUT
    120
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
