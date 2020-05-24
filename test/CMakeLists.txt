set(SUBDIRS "emqx" "onenet" "baidu")

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
