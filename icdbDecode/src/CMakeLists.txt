file(GLOB src *.c)
add_executable(icdbDecode ${src})

add_subdirectory(common)
add_subdirectory(cdbblks)
add_subdirectory(cdbcatlg)
add_subdirectory(cdbcmpcache)
