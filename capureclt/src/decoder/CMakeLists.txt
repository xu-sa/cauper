
#decoder

add_library(lib1 STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scdecoder.c
)

target_include_directories(lib1 PUBLIC
    ${FFMPEG_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(lib1 PROPERTIES
        OUTPUT_NAME "scdecoder"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib1 PUBLIC c_std_99)

