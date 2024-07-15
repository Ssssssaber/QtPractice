cmake_minimum_required(VERSION 3.5)
project(QtPractice VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)
message(STATUS ${QT_VERSION_MAJOR})
if(6 GREATER_EQUAL 6)
    qt_add_executable(QtPractice
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        server.h server.cpp
        client.h client.cpp
        dataprocessor.h dataprocessor.cpp
        xyzcircuitdata.h
        chartwidget.h chartwidget.cpp
        chartmanager.h chartmanager.cpp

        windowworker.h windowworker.cpp
        cdrworker.h cdrworker.cpp




        cmake/FindLibUdev.cmake cmake/FindUSB1.cmake
        circuitdatareceiver.cpp
        circuitdatareceiver.h

        cdrworker.h cdrworker.cpp
        linux-system-usage.hpp
        perfomancechecker.h perfomancechecker.cpp
        circuitconfiguratorwidget.h circuitconfiguratorwidget.cpp
        CircuitConfiguration.h
        collapseelement.h collapseelement.cpp
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

target_link_libraries(QtPractice PRIVATE Qt6::Widgets)

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

find_library(LIBNII libnii.a /usr/local/lib/)
if (${LIBNII} STREQUAL LIBP7-NOTFOUND)
    message(FATAL_ERROR "no libii")
else()
    message(STATUS "libnii found")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBNII})
include_directories(libnii/Headers)

# adding network components
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Network)
find_package(Qt6 REQUIRED COMPONENTS Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Network)

# adding charts
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Charts)

# adding libusb components
SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_package(USB1 REQUIRED)
find_package(LibUdev)

target_include_directories( ${PROJECT_NAME}
                            PUBLIC ${LIBUSB_INCLUDE_DIR}
                            PRIVATE . ${NII_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME}
                       PRIVATE
                           nii
                       PUBLIC
                           ${LIBUSB_LIBRARIES}
                           ${LIBUDEV_LIBRARIES}
                           pthread rt )
