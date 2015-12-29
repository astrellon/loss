cmake_minimum_required(VERSION 2.8)

project(loss)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(EXECUTABLE_NAME "LossProg")
set(EXECUTABLE_NAME_FUSE "LossFuse")

set(CMAKE_CXX_FLAGS "-std=c++11 -g -O0 -I./")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g -O0 -I./")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O2 -I./")

file(GLOB_RECURSE loss_headers "loss/*.h")
file(GLOB_RECURSE loss_sources "loss/*.cpp")

file(GLOB_RECURSE lua_headers "lua/*.h")
file(GLOB_RECURSE lua_sources "lua/*.c")

set(loss_include_dirs "")
foreach (_file ${loss_headers})
    get_filename_component(_dir ${_file} PATH)
    list (APPEND loss_include_dirs ${_dir})
endforeach()

foreach (_file ${lua_headers})
    get_filename_component(_dir ${_file} PATH)
    list (APPEND loss_include_dirs ${_dir})
endforeach()
list(REMOVE_DUPLICATES loss_include_dirs)

include_directories(${loss_include_dirs})

find_package(Boost 1.58 REQUIRED system context coroutine)
include_directories(${Boost_INCLUDE_DIR})

add_executable(${EXECUTABLE_NAME} "main.cpp" ${loss_sources} ${lua_sources})

target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${EXECUTABLE_NAME} ncurses)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(FUSE REQUIRED)

include_directories(${FUSE_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
add_executable(${EXECUTABLE_NAME_FUSE} "lossfs.cpp" ${loss_sources} ${lua_sources})
target_link_libraries(${EXECUTABLE_NAME_FUSE} ${FUSE_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME_FUSE} ${Boost_LIBRARIES})

# TODO Fix so this isn't needed.
target_link_libraries(${EXECUTABLE_NAME_FUSE} ncurses)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

