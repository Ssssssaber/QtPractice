cmake_minimum_required(VERSION 3.5)
project(QtPractice VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtPractice
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        server.h server.cpp
        client.h client.cpp
        dataprocessor.h dataprocessor.cpp
        xyzcircuitdata.h
        chartwidget.h chartwidget.cpp
        chartmanager.h chartmanager.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QtPractice APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QtPractice SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QtPractice
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(QtPractice PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QtPractice)
endif()
set_target_properties(QtPractice PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QtPractice
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtPractice)
endif()
include(FetchContent)

find_library(LIBP7 libp7.a /usr/local/lib/)
if (${LIBP7} STREQUAL LIBP7-NOTFOUND)
    message(FATAL_ERROR "no libp7")
else()
    message(STATUS "libp7 found")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBP7})
include_directories(libp7/Headers)

# adding network components
find_package(Qt6 REQUIRED COMPONENTS Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Network)

# adding charts
find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Charts)
