cmake_minimum_required (VERSION 3.15)
project(dbPool)

# 定义源码文件
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# 指定头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 指定编译结果输出路径
set(EXECUTABLE_OUTPUT_PATH ./)
# 指定c++版本
set(CMAKE_CXX_STANDARD 11)

add_executable(dbPool.out ${SRC})


# 若在别的项目中需要连接非系统库的动态库则添加下面代码
## link_directories(指定所在的路径)

find_package(Threads REQUIRED) 

target_link_libraries(dbPool.out mysqlcppconn Threads::Threads)
