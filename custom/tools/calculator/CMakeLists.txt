cmake_minimum_required(VERSION 3.25)
project(
    CaacCalculator
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets Qml)
qt_standard_project_setup()

add_library(CalculatorCore STATIC CalculatorEngine.cc CalculatorEngine.h CalculatorWindow.cc CalculatorWindow.h)
target_link_libraries(CalculatorCore PUBLIC Qt6::Widgets Qt6::Qml)
target_include_directories(CalculatorCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Embed the exact calculation code used by the ground station, rather than a second implementation.
qt_add_resources(
    CalculatorCore calculator_math
    PREFIX "/calculator"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/../../src/PlanView"
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../src/PlanView/CaacCalculatorMath.js"
)
qt_add_resources(
    CalculatorCore calculator_icon
    PREFIX "/calculator"
    FILES calculator.ico
)

qt_add_executable(CaacCalculator main.cc)
target_link_libraries(CaacCalculator PRIVATE CalculatorCore)
target_compile_definitions(CaacCalculator PRIVATE CAAC_CALCULATOR_VERSION="${PROJECT_VERSION}")
if(MSVC)
    target_compile_options(CalculatorCore PRIVATE /utf-8 /W4)
    target_compile_options(CaacCalculator PRIVATE /utf-8 /W4)
endif()
if(WIN32)
    target_sources(CaacCalculator PRIVATE calculator.rc)
    set_target_properties(CaacCalculator PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    qt_add_executable(CalculatorTest CalculatorTest.cc)
    target_link_libraries(CalculatorTest PRIVATE CalculatorCore Qt6::Test)
    if(MSVC)
        target_compile_options(CalculatorTest PRIVATE /utf-8 /W4)
    endif()
    add_test(NAME CalculatorTest COMMAND CalculatorTest)
    set_tests_properties(CalculatorTest PROPERTIES LABELS "Unit" TIMEOUT 60 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

install(TARGETS CaacCalculator RUNTIME DESTINATION bin)
install(FILES README.md NOTICE_zh-CN.txt DESTINATION .)
install(FILES ../../../LICENSE-GPL ../../../LICENSE-APACHE DESTINATION licenses)
if(WIN32)
    qt_generate_deploy_app_script(
        TARGET
        CaacCalculator
        OUTPUT_SCRIPT
        deploy_script
        NO_TRANSLATIONS
        NO_COMPILER_RUNTIME
        DEPLOY_TOOL_OPTIONS
        --no-opengl-sw
        --no-system-d3d-compiler
    )
    install(SCRIPT "${deploy_script}")
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin)
    include(InstallRequiredSystemLibraries)
endif()
