cmake_minimum_required(VERSION 2.8)

set(MBEDTLS_SUBDIRS "library" "include")
set(MBEDTLS_INCDIRS "include" "include/mbedtls" "configs")

foreach(incdir ${INCDIRS})
    include_directories(${MBEDTLS_INCDIRS})
endforeach()

foreach(subdir ${MBEDTLS_SUBDIRS})
    add_subdirectory(${subdir})
endforeach()



