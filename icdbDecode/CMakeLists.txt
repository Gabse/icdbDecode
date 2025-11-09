cmake_minimum_required(VERSION 3.21)
project("icdbDecode")

# Add source to this project's executable.
add_subdirectory(src)

# Add zlib
add_subdirectory(lib/zlib)
target_link_libraries(icdbDecode zlibstatic)
