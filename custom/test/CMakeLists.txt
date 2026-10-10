target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorTest.cc
                                             ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorUITest.cc
)
add_qgc_test(CaacRouteEditorTest LABELS Unit MissionManager)
if(WIN32)
    # The Windows offscreen plugin has no system font database; use the native platform for real UI geometry.
    set(_caac_ui_env ENV_MODIFICATION "QT_QPA_PLATFORM=set:windows")
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
