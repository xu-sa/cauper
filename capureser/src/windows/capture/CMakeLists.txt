add_library(lib1 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sccap.c
)
target_include_directories(lib1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(lib1 PROPERTIES
        OUTPUT_NAME "sccap.lib"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib1 PUBLIC c_std_99)