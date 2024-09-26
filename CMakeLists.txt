cmake_minimum_required(VERSION 3.20.0)
project(main)
set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include())
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(
    main
    ${SRC_LIST}
)
# 添加 test 可执行文件 
add_executable( 
    test 
    ${PROJECT_SOURCE_DIR}/test.cpp 
)