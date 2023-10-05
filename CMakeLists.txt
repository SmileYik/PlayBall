cmake_minimum_required(VERSION 3.5)

project(Ball VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_definitions(DEBUG_FLAG=OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set( 
    BALL_SOURCE
    src/main.cpp
    src/mainwindow.cpp
    src/gamewindow.cpp
    src/widget/glwidget.cpp
    src/entity/aentity.cpp
    src/entity/entityfood.cpp
    src/entity/player.cpp
    src/shape/circular.cpp
    src/shape/foodshape.cpp
    src/network/client.cpp
    src/network/server.cpp
    src/network/socket/ioreader.cpp
    src/network/packet/ipacket.cpp
    src/simplegame.cpp
)

set(
    BALL_HEADER
    src/mainwindow.h
    src/gamewindow.h
    src/widget/glwidget.h
    src/interface/icontrollable.h
    src/interface/ientity.h
    src/interface/irenderable.h
    src/interface/ishape.h
    src/interface/iupdatable.h
    src/util/point.h
    src/entity/aentity.h
    src/entity/player.h
    src/shape/circular.h
    src/network/client.h
    src/network/server.h
)

set(
    BALL_UI
    src/mainwindow.ui
)

set(PROJECT_SOURCES
    ${BALL_SOURCE}
    ${BALL_HEADER}
    ${BALL_UI}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Ball
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Ball APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID) 
        add_library(Ball SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Ball
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Ball PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Ball)
endif()
set_target_properties(Ball PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Ball
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Ball)
endif()
