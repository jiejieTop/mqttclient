
macro(install_project)
    get_module_info()
    foreach(dir ${MODULE_INCLUDE_DIRS})
        string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" dir ${dir})
        set(install_dir ${CMAKE_INSTALL_PREFIX}/include/${dir})
        string(REGEX REPLACE "//" ";/" install_dir ${install_dir})
        set(${PROJECT_NAME}_INCLUDE_DIRS_INSTALL ${${PROJECT_NAME}_INCLUDE_DIRS_INSTALL};\n${install_dir})
    endforeach()
    
    set(${PROJECT_NAME}_LINK_DIRS_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
    set(${PROJECT_NAME}_LIBRARIES_INSTALL ${MODULE_NAME})
    set(${PROJECT_NAME}_EXECUTABLE_INSTALL ${PROJECT_NAME})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in 
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake)
    
    install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake)
endmacro()

