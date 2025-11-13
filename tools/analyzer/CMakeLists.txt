cmake_minimum_required(VERSION 3.21)
project("icdbAnalyzer")

# Add source to this project's executable.
add_executable(icdbAnalyzer "src/main.c" "../../icdbDecode/src/common.c" "../../icdbDecode/src/stringutil.c" "../../icdbDecode/src/log.c")
