
#encoder
 
add_library(lib2 STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scencoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scencoder_utils.c
)

target_include_directories(lib2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${AVCODEC_INC}
    ${AVUTIL_INC}
    ${SWSCALE_INC}
)
set_target_properties(lib2 PROPERTIES
        OUTPUT_NAME "scencoder"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib2 PUBLIC c_std_99)

