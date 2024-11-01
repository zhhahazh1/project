cmake_minimum_required(VERSION 3.20.0)
project(main)
set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include())
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
file(GLOB TEST_SRC_LIST ${PROJECT_SOURCE_DIR}/test/*.cpp) # 查找 test 目录下的所有 .cpp 文件

add_executable(
    partitioner
    ${SRC_LIST}
)
# 添加测试可执行文件
foreach(test_src ${TEST_SRC_LIST})
    get_filename_component(test_name ${test_src} NAME_WE) # 获取文件名（不带扩展名）
    add_executable(${test_name} ${test_src}) # 创建可执行文件
endforeach()