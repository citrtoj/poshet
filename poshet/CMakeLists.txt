cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(poshet VERSION 1.0)

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
find_package(SQLite3 REQUIRED)


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
    src/MailCreatorFrame.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${wxWidgets_INCLUDE_DIRS})
message(${wxWidgets_LIBRARIES})

target_compile_definitions(${CMAKE_PROJECT_NAME}  PUBLIC ${wxWidgets_DEFINITIONS})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${wxWidgets_CXX_FLAGS})
target_link_libraries(${CMAKE_PROJECT_NAME} 
  ${wxWidgets_LIBRARIES}
  sqlite3
  pthread
  dl
)