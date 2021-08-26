cmake_minimum_required(VERSION 3.17)

project(access_control_app VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(/opt/gaia/cmake/gaia.cmake)

find_package(Threads REQUIRED)

# Generate C++ headers from the .ddl schema file.
process_schema(DDL_FILE ${PROJECT_SOURCE_DIR}/src/access_control.ddl)

# Translate the declarative .ruleset file into a C++ source file.
translate_ruleset(
  RULESET_FILE
    ${PROJECT_SOURCE_DIR}/src/access_control.ruleset
  CLANG_PARAMS
    -I ${PROJECT_SOURCE_DIR}/include
    -I ${GENERATED_OUTPUTS}
)

add_executable(access_control
  src/main.cpp
  src/helpers.cpp
  src/actions.cpp
  src/ui_messaging.cpp
)

target_add_gaia_generated_sources(access_control)
add_dependencies(access_control translate_access_control_ruleset)

target_include_directories(access_control
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    /opt/gaia/include
)

target_link_libraries(access_control
  PRIVATE
    gaia
    Threads::Threads
)
