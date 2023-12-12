cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(poshet VERSION 1.0)


# --- Uncomment the next line and comment the `find_package` one in case you would rather build wxWidgets from source, after downloading its source code using `git submodule update --init` 
add_subdirectory(thirdparty/wxWidgets)
# find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)

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
  ${wxWidgets_LIBRARIES}
  SQLiteCpp
  sqlite3
  pthread
  dl

  sfml-graphics
  sfml-window
  sfml-system
)