# CMakeList.txt: common-tool 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
project(test)


set(CMAKE_CXX_STANDARD 23)

add_executable(test common-tool-test.cpp)

target_link_libraries(test common-tool)

# TODO: 如有需要，请添加测试并安装目标。
