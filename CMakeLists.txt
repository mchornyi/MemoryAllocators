cmake_minimum_required (VERSION 3.16)
project (MemoryAllocators LANGUAGES CXX)

find_package( Threads )

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(main ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main )

set(EXECUTABLE_OUTPUT_PATH "bin")

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

target_compile_features(main PRIVATE cxx_std_11)