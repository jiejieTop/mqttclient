cmake_minimum_required(VERSION 2.8)

project(mqtt-client)

set(TARGETS "mqtt-client")
set(SUBDIRS  "common" "mqtt" "platform" "network" "mqttclient")

# 选项
option(BUILD_EXAMPLES "Whether to enable examples" ON)

# 库文件前缀
set(LIBRARY_PREFIX "mc")

# 设置版本信息
set(PROJECT_MAJOR_VERSION 1) # 架版本信息
set(PROJECT_MINOR_VERSION 2) # 主版本信息
set(PROJECT_PATCH_VERSION 0) # 次版本信息
set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT_DIR}/build/bin)       #设置可执行文件的输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_ROOT_DIR}/build/lib)           #设置库文件的输出目录
link_directories(${LIBRARY_OUTPUT_PATH})


set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS "-lpthread")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g -no-pie -ggdb")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -DNDEBUG")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11")
    set(CMAKE_CXX_FLAGS "-lpthread")
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g -no-pie -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3") 
endif(CMAKE_COMPILER_IS_GNUCXX)


#########################################
## load cmake config and load 3rdparty ##
#########################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/color.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/module.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)

# foreach(incdir ${INCDIRS})
#     include_directories(${incdir})
# endforeach()

include_directories(${PROJECT_ROOT_DIR}/config)

foreach(subdir ${SUBDIRS})
    add_subdirectory(${PROJECT_ROOT_DIR}/${subdir})
endforeach()


get_module_info()
show_module_info()
install_project()


if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
