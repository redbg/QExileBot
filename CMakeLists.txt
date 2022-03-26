cmake_minimum_required(VERSION 3.20)

project(QExileBot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(QExileBot
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui

    Account.cpp
    Account.h

    AccountModel.cpp
    AccountModel.h

    ExileSocket.cpp
    ExileSocket.h

    Global.h
    Helper.h
    QExileBot.qrc
)

# Qt6::Widgets
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(QExileBot PRIVATE Qt6::Widgets)
# Qt6::Network
find_package(Qt6 REQUIRED COMPONENTS Network)
target_link_libraries(QExileBot PRIVATE Qt6::Network)
# Qt6::WebEngineWidgets
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
target_link_libraries(QExileBot PRIVATE Qt6::WebEngineWidgets)

# vcpkg
include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
# cryptopp
find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(QExileBot PRIVATE cryptopp-static)
