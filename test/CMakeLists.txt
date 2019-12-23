aux_source_directory(. DIR_SRCS)

add_executable(${TARGETS} ${DIR_SRCS})

foreach(findlib ${LIBNAMES})
    target_link_libraries(${TARGETS} ${findlib})
endforeach()

find_package("Threads")
target_link_libraries(${TARGETS} ${CMAKE_THREAD_LIBS_INIT})