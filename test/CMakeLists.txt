set(SUBDIRS "emqx")

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
