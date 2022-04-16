cmake_minimum_required(VERSION 3.20)

project(QExileBot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
find_package(Qt6 REQUIRED COMPONENTS Qml)

add_executable(QExileBot
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui

    Account.cpp
    Account.h

    AccountModel.cpp
    AccountModel.h

    Character.h
    CharacterModel.h

    ExileSocket.cpp
    ExileSocket.h

    Packet.h
    PacketField.h
    PacketListModel.h
    
    ExileClient.cpp
    ExileClient.h

    ExileGame.cpp
    ExileGame.h

    GameObject.h
    GameObjectScene.h

    Item.cpp
    Item.h

    Global.h
    
    Helper.h
    Helper.cpp

    AStar/astar.cpp
    AStar/astar.h
    AStar/blockallocator.cpp
    AStar/blockallocator.h

    QExileBot.qrc
)

if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()


target_link_libraries(QExileBot PRIVATE Qt6::Widgets)
target_link_libraries(QExileBot PRIVATE Qt6::Network)
target_link_libraries(QExileBot PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(QExileBot PRIVATE Qt6::Qml)

# vcpkg
include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
# cryptopp
find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(QExileBot PRIVATE cryptopp-static)
