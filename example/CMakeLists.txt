cmake_minimum_required(VERSION 2.8)
project(mqttclient_example)

set(EXAMPLE_DIRS "emqx" "onenet" "baidu" "ali")

# #########################
# ## find depend package ##
# #########################
if(NOT BUILD_EXAMPLES)
    include(${CMAKE_CURRENT_SOURCE_DIR}/mqttclient.cmake)
    set(PACKAGE_DEPEND ${MQTTCLIENT_PACKAGE_LIBS})
endif()

################
## add subdir ##
################
foreach(example ${EXAMPLE_DIRS})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${example})
endforeach()
