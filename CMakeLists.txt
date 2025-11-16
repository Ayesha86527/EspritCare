cmake_minimum_required(VERSION 3.16)

project(final VERSION 0.1 LANGUAGES CXX)

# --- Qt Setup ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Qt ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Source Files ---
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dashboardwindow.cpp
    dashboardwindow.h
    addpatientwindow.cpp
    addpatientwindow.h
    addsessionwindow.cpp
    addsessionwindow.h
    viewpatientwindow.cpp
    viewpatientwindow.h
    espritdb.cpp
    espritdb.h
    dsabackend.h
    dsabackend.cpp
)

# --- Target Setup ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(final
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        dsabackend.h dsabackend.cpp
    )
else()
    if(ANDROID)
        add_library(final SHARED ${PROJECT_SOURCES})
    else()
        add_executable(final ${PROJECT_SOURCES})
    endif()
endif()

# --- Link Qt Libraries ---
target_link_libraries(final PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)
target_link_libraries(final PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# --- Bundle / Executable Properties ---
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.final)
endif()

set_target_properties(final PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Install Rules ---
include(GNUInstallDirs)
install(TARGETS final
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --- Qt6 Finalization ---
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(final)
endif()

