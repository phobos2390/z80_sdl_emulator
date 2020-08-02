cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_helpers/")

set(PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_ROOT_DIR ${PROJ_DIR}/src/${PROJ_NAME}/${REPO_NAME})

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_FLAGS "-g -m64")
set(CMAKE_CXX_FLAGS "-g -m64 -std=c++11")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

include_directories("${PROJ_DIR}/z/API")
include_directories("${PROJ_DIR}/z80/API")

include_directories("${PROJ_DIR}/src/")
include_directories("${PROJ_DIR}/src/phobos2390/")
include_directories("${PROJ_DIR}/catch2/single_include")

add_subdirectory("${PROJ_DIR}/src/constants")

set(Z80_SOURCE_DIRECTORIES ${PROJ_DIR}/z80/)

set(z80_source_files "")
foreach(source_dir ${Z80_SOURCE_DIRECTORIES})
  message("Finding everything in ${source_dir}")
  file(GLOB_RECURSE dir_src_files ${source_dir}/*.c)
  set(z80_source_files ${z80_source_files}
                       ${dir_src_files})
endforeach()

message("Building z80 source emulator files ${z80_source_files}")
add_library(z80_emu_library SHARED ${z80_source_files})

set(pz80e_src_root src/phobos2390/z80_emulator)
set(pz80e_src .)
set(pz80e_test_files)
set(pz80e_lib_files)

foreach(source_dir ${pz80e_src})
  message("finding everything in ${pz80e_src_root}/${source_dir}")
  file(GLOB dir_src_files ${pz80e_src_root}/${source_dir}/*.cpp)
  set(pz80e_lib_files ${pz80e_lib_files}
                      ${dir_src_files})
  file(GLOB globbed_test_src ${pz80e_src_root}/${source_dir}/*.cpp
                             ${pz80e_src_root}/${source_dir}/test/*.cpp)
  set(pz80e_test_files ${pz80e_test_files}
                       ${globbed_test_src})
endforeach()

#set(pz80e_test_files 
#                     src/phobos2390/test_catch_main/catch_definition_test.cpp
#                     ${pz80e_test_files})

add_library(pz80e_lib STATIC ${pz80e_lib_files})
add_executable(test_pz80e ${pz80e_test_files})
target_link_libraries(test_pz80e pz80e_lib z80_emu_library ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_executable(z80_emulator src/z80_emulator/emulator.cpp)
target_link_libraries(z80_emulator pz80e_lib z80_emu_library ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
