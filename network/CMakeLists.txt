string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_LIB_NAME ${CMAKE_CURRENT_SOURCE_DIR}) 

set(CURRENT_LIB_NAME ${LIBRARY_PREFIX}-${CURRENT_LIB_NAME})

set(SUBDIRS "mbedtls")

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()

set(DEPEND_LIBRARY_INTERNAL "common" "log" "platform" "mbedtls")

# 指定头文件搜索目录
set(${CURRENT_LIB_NAME}_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 源文件目录
set(${CURRENT_LIB_NAME}_SRC_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_module_info(${CURRENT_LIB_NAME} "${${CURRENT_LIB_NAME}_INC_DIRS}")
get_module_info()



foreach(FOREACH_SRC_DIR ${${CURRENT_LIB_NAME}_SRC_DIRS})
    aux_source_directory(${FOREACH_SRC_DIR} ${CURRENT_LIB_NAME}_SOURCES)
endforeach()


foreach(lib ${DEPEND_LIBRARY_INTERNAL})
    list(APPEND DEPEND_LIBRARY ${LIBRARY_PREFIX}-${lib})
    list(APPEND DEPEND_LIBRARY_STATIC lib${LIBRARY_PREFIX}-${lib}.a)
endforeach()

# 生成动态库
add_library(${CURRENT_LIB_NAME}        SHARED ${${CURRENT_LIB_NAME}_SOURCES})
target_link_libraries(${CURRENT_LIB_NAME} ${DEPEND_LIBRARY})
# 生成静态库
add_library(${CURRENT_LIB_NAME}_static STATIC ${${CURRENT_LIB_NAME}_SOURCES})
target_link_libraries(${CURRENT_LIB_NAME}_static ${DEPEND_LIBRARY_STATIC})

# 进行必要的配置，以生成动态库和静态库
# 指定静态库的输出名称
set_target_properties(${CURRENT_LIB_NAME}_static PROPERTIES OUTPUT_NAME ${CURRENT_LIB_NAME})
# 使动态库和静态库同时存在
set_target_properties(${CURRENT_LIB_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CURRENT_LIB_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
# 指定动态库版本
set_target_properties(${CURRENT_LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# 打包相关 install package
install(TARGETS ${CURRENT_LIB_NAME} ${CURRENT_LIB_NAME}_static
    EXPORT ${CURRENT_LIB_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

install(DIRECTORY ${${CURRENT_LIB_NAME}_INC_DIRS}
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "CMakeLists.txt" EXCLUDE)
