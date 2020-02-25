cmake_minimum_required(VERSION 2.8)

project(mqtt-client)

set(TARGETS "mqtt-client")
set(SUBDIRS "mqtt" "test" "platform" "network" "common" "mqttclient")
set(INCDIRS "mqtt" "platform/linux" "common" "common/log" "common/mbedtls/include" "network" "mqttclient" "test")
set(OUTDIRS "build")
set(LIBNAMES "mqtt" "platform" "network" "common" "salof" "mbedtls" "mqttclient")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "clang++" )
set(CMAKE_BUILE_TYPE "RELEASE")
set(PROJECT_ROOT_PATH "${PROJECT_SOURCE_DIR}")
set(LIBRARY_OUTPUT_PATH "${PROJECT_ROOT_PATH}/${OUTDIRS}/lib/")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/${OUTDIRS}/bin/")

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O1 -DNDEBUG ")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11")
    set(CMAKE_CXX_FLAGS "-lpthread")
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
endif(CMAKE_COMPILER_IS_GNUCXX)

foreach(incdir ${INCDIRS})
    include_directories(${incdir})
endforeach()

foreach(subdir ${SUBDIRS})
    add_subdirectory(${PROJECT_ROOT_PATH}/${subdir})
endforeach()

link_directories(${LIBRARY_OUTPUT_PATH})

