# CMakeList.txt : CMake project for icdbDecode, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.24)

project ("icdbDecode")

# Add source to this project's executable.
add_executable (icdbDecode "src/main.c" "src/log.c" "src/common.c" "src/stringutil.c" "src/unpack.c" "src/parser.c" "src/catlgatl.c" "src/cdbblks.c" "src/kicad.c")

# Add zlib
 add_subdirectory(lib/zlib-1.3.1)
 target_link_libraries(icdbDecode zlib)
 add_custom_command(
        TARGET icdbDecode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/lib/zlib-1.3.1/zlibd.dll
                ${CMAKE_CURRENT_BINARY_DIR}/zlibd.dll)
                
# Add test file
add_custom_command(
        TARGET icdbDecode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove
                ${CMAKE_CURRENT_SOURCE_DIR}/icdb.dat)
add_custom_command(
        TARGET icdbDecode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/files/icdb.dat
                ${CMAKE_CURRENT_BINARY_DIR}/icdb.dat)