# aux_source_directory(. DIR_SRCS)

# set(INCDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# add_executable("onenet" ${DIR_SRCS})

# foreach(findlib ${LIBNAMES})
#     target_link_libraries("onenet" ${findlib})
# endforeach()

# find_package("Threads")
# target_link_libraries("onenet" ${CMAKE_THREAD_LIBS_INIT})

cmake_minimum_required(VERSION 2.8)
project(mqttclient_example)

##################
## set arg info ##
##################
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

#########################
## find depend package ##
#########################
if(BUILD_EXAMPLES)
    set(PACKAGE_DEPEND ${MODULE_NAME})
endif()

find_package("Threads")

###########
## build ##
###########

# 指定头文件搜索目录
set(INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 指定依赖文件搜索目录
set(DEPEND_SRC_DIRS
)

# 指定应用文件搜索目录
set(EXAMPLE_SRC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

foreach(FOREACH_SRC_DIR ${DEPEND_SRC_DIRS})
    aux_source_directory(${FOREACH_SRC_DIR} DEPEND_SOURCES)
endforeach()

foreach(FOREACH_SRC_DIR ${EXAMPLE_SRC_DIRS})
    aux_source_directory(${FOREACH_SRC_DIR} EXAMPLE_SOURCES)
endforeach()

include_directories(${INC_DIRS})

foreach(example_src_name ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_src_name} NAME_WE)
    add_executable(${example_name}_example ${example_src_name} ${DEPEND_SOURCES})
    target_link_libraries(${example_name}_example ${CMAKE_THREAD_LIBS_INIT} ${PACKAGE_DEPEND})
    message(STATUS "build example : ${example_name} ")
endforeach()


