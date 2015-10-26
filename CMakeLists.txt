cmake_minimum_required(VERSION 2.6)

project(loss)

set(EXECUTABLE_NAME "LossProg")
set(EXECUTABLE_NAME_FUSE "LossFuse")

set(CMAKE_CXX_FLAGS "-std=c++11 -g -O0 -I./")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
#add_compile_options(-std=c++11 -g -O0 -I./)

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

add_executable(${EXECUTABLE_NAME} "main.cpp" ${loss_sources} ${lua_sources})

find_package(Threads REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} Threads::Threads)
target_link_libraries(${EXECUTABLE_NAME} ncurses)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(FUSE REQUIRED)

include_directories(${FUSE_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
add_executable(${EXECUTABLE_NAME_FUSE} "lossfs.cpp" ${loss_sources} ${lua_sources})
target_link_libraries(${EXECUTABLE_NAME_FUSE} ${FUSE_LIBRARIES})

# TODO Fix so this isn't needed.
target_link_libraries(${EXECUTABLE_NAME_FUSE} ncurses)
