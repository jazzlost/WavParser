# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10.0)

# 项目名称
project(WavParser)

# 设置C/C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件路径
include_directories("Inc")

# 枚举头文件
file(GLOB_RECURSE INCLUDES "Inc/*.h" "Inc/*.hpp")

# 枚举源文件
aux_source_directory("Src" SOURCES)

# 输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 生成可执行的文件
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})