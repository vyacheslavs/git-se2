cmake_minimum_required(VERSION 3.16)

project(git-se2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup()

qt_add_executable(appgit-se2
    main.cpp
)

qt_add_qml_module(appgit-se2
    URI git-se2
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES error-handling.h error-handling.cpp program_options.h program_options.cpp
        SOURCES repository.h repository.cpp
)

set(BUILD_TESTS OFF)
set(BUILD_CLI OFF)
set(USE_HTTPS OFF)
set(USE_NTLMCLIENT OFF)
set(EXPECTED_BUILD_PACKAGE OFF)
set(BUILD_TESTING OFF)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_CUDA_TEST OFF)

add_subdirectory(libgit2)
add_subdirectory(expected)
add_subdirectory(libfmt)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appgit-se2 PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appgit-se2
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appgit-se2 PRIVATE ${PROJECT_SOURCE_DIR}/libgit2/include)
target_link_libraries(appgit-se2
    PRIVATE Qt6::Quick Qt6::QuickControls2 libgit2package expected fmt
)

include(GNUInstallDirs)
install(TARGETS appgit-se2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
