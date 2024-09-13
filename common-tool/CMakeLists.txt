# CMakeList.txt: common-tool 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。

include_directories ("inc")
include_directories ("include/cryptopp890")

file (GLOB SRC_FILES "src/*.cpp")

if (MSVC)

set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /MTd")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/DEBUG /OPT:REF /OPT:ICF")

set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MT /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF")

elseif (CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_FLAGS_DEBUG "-O3")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG )
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG )

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE)


#设置编译DEBUG编译选项
endif()
#判断是否为64位系统
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH "x64")
else()
  set(ARCH "x86")
endif()
#是否为debug模式
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(MODE "Debug")
else()
  set(MODE "Release")
endif()

#设置编译输出目录

link_directories ("lib/${ARCH}/${MODE}")
add_executable (common-tool ${SRC_FILES})
target_link_libraries (common-tool "cryptlib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET common-tool PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
