# 通常要写的
cmake_minimum_required(VERSION 3.20) #设置cmake最低要求
project(
    main      #项目名称
    LANGUAGES CXX C     #编程语言
    DESCRIPTION "海康相机SDK封装"
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Release" ) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制要求使用C++ 23

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# 获取 CMakeLists.txt 文件所在目录的绝对路径
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DIR}/bin)
# 四个线程编译
set(CMAKE_BUILD_PARALLEL_LEVEL 4) 

find_package(OpenCV REQUIRED)

link_directories(${PROJECT_DIR}/lib)
include_directories(${PROJECT_DIR}/include)
include_directories(${PROJECT_DIR}/include/MVS)
#******************************************************************
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
#******************************************************************
target_link_libraries(${CMAKE_PROJECT_NAME}  MvCameraControl)
target_link_libraries(${CMAKE_PROJECT_NAME}  ${OpenCV_LIBRARIES})