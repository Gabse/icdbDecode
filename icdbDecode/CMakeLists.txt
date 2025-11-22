cmake_minimum_required(VERSION 3.21)
project("icdbDecode")

# Add source to this project's executable.
add_subdirectory(src)

# Add zlib
add_subdirectory(lib/zlib)
target_link_libraries(icdbDecode zlibstatic)

# Add test file
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/files/icdb.dat
		DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/src)
endif()
