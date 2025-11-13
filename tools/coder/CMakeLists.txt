cmake_minimum_required(VERSION 3.21)
project("icdbCoder")

# Add source to this project's executable.
add_executable(icdbCoder "src/main.c" "../../icdbDecode/src/common.c" "../../icdbDecode/src/stringutil.c" "../../icdbDecode/src/log.c" "../../icdbDecode/src/list.c")
