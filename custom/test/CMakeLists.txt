target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorTest.cc
                                             ${CMAKE_CURRENT_SOURCE_DIR}/CaacRouteEditorUITest.cc
)
add_qgc_test(CaacRouteEditorTest LABELS Unit MissionManager)
add_qgc_test(
    CaacRouteEditorUITest
    LABELS
    Integration
    MissionManager
    NoSanitizer
    TIMEOUT
    120
)
