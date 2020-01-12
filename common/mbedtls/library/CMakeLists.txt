aux_source_directory(. MBEDTLS_SRCS)

add_library("mbedtls" STATIC ${MBEDTLS_SRCS})
target_link_libraries("mbedtls" "wrapper")
