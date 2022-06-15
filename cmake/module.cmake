set(MODULE_NAME "")
set(STATIC_MODULE_NAME "")
set(MODULE_INCLUDE_DIRS "")

# describe property 
define_property(GLOBAL PROPERTY MODULE_NAME_LIST BRIEF_DOCS "MODULE_NAME_LIST" FULL_DOCS "module name list") 
define_property(GLOBAL PROPERTY MODULE_INCLUDE_DIRS_LIST BRIEF_DOCS "MODULE_INCLUDE_DIRS_LIST" FULL_DOCS "module include directory list") 

# initialize property 
set_property(GLOBAL PROPERTY MODULE_NAME_LIST "")
set_property(GLOBAL PROPERTY MODULE_INCLUDE_DIRS_LIST "")


# macro for add values into the list
macro(set_module_info module var)     
    set_property(GLOBAL APPEND PROPERTY MODULE_NAME_LIST ${module})
    set_property(GLOBAL APPEND PROPERTY MODULE_INCLUDE_DIRS_LIST ${var}) 
endmacro(set_module_info)

macro(set_header_info var)     
    set_property(GLOBAL APPEND PROPERTY MODULE_INCLUDE_DIRS_LIST ${var}) 
endmacro(set_header_info)

macro(get_module_info)     
    get_property(TMP_MODULE_NAME GLOBAL PROPERTY MODULE_NAME_LIST)
    get_property(TMP_INC_DIR GLOBAL PROPERTY MODULE_INCLUDE_DIRS_LIST)
    set(MODULE_NAME ${TMP_MODULE_NAME})
    set(MODULE_INCLUDE_DIRS ${TMP_INC_DIR})
    foreach(module ${MODULE_NAME})
        # 将 *.so 替换为 *.a
        string(REGEX REPLACE ".so" ".a" module ${module})
        set(STATIC_MODULE_NAME "${STATIC_MODULE_NAME};lib${module}")
    endforeach()
    
    include_directories(${MODULE_INCLUDE_DIRS})
endmacro(get_module_info)

macro(show_module_info)
    message(INFO "module :")
    foreach(module ${MODULE_NAME})
    message(STATUS ${module})
    endforeach()

    message(INFO "static module :")
    foreach(module ${STATIC_MODULE_NAME})
    message(STATUS ${module})
    endforeach()

    message(INFO "module include directories is :")
    foreach(include_dir ${MODULE_INCLUDE_DIRS})
    message(STATUS ${include_dir})
    endforeach()
endmacro(show_module_info)



