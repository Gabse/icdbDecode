file(GLOB src *.c)
add_executable(icdbDecode ${src})

add_subdirectory(cdbblks)
add_subdirectory(cdbcatlg)