# CMakeList.txt: common-tool 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。

set(CMAKE_CXX_STANDARD 23)

add_library (${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc/common-tool.hpp)
target_link_libraries (${PROJECT_NAME})
target_include_directories (${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# TODO: 如有需要，请添加测试并安装目标。
