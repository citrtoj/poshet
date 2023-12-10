cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(poshet VERSION 1.0)

add_subdirectory(thirdparty/wxWidgets)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

set(SOURCES
    src/main.cpp
    src/Config.cpp
    src/Utils.cpp
    src/App.cpp
    src/AppController.cpp
    src/Session.cpp
    src/ConnectionBase.cpp
    src/SmtpConnection.cpp
    src/Pop3Connection.cpp
    src/DatabaseConnection.cpp
    src/LoginData.cpp
    src/LoginFrame.cpp
    src/DashboardFrame.cpp
    src/NewMailDialog.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wxWidgets/include)

target_link_libraries(${CMAKE_PROJECT_NAME} 
  wx::net wx::core wx::base wx::richtext wx::html
  SQLiteCpp
  sqlite3
  pthread
  dl

  sfml-graphics
  sfml-window
  sfml-system
)