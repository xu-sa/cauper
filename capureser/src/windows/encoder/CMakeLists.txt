add_library(lib2 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scwencoder.c
)
target_include_directories(lib2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(lib2 PROPERTIES
        OUTPUT_NAME "scwencoder.lib"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib2 PUBLIC c_std_99)