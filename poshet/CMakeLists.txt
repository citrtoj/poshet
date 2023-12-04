cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(poshet VERSION 1.0)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

set(SOURCES
    src/main.cpp
    src/GUIComponents.cpp
    src/Utils.cpp
    src/Pop3Connection.cpp
    src/DatabaseConnection.cpp
    src/Config.cpp
    src/App.cpp
    src/LoginView.cpp
    src/LoginController.cpp
    src/LoginModel.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

message(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${CMAKE_PROJECT_NAME}
  SQLiteCpp
  sqlite3
  pthread
  dl

  sfml-graphics
  sfml-window
  sfml-system
)