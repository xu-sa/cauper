add_library(lib3 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scwinput.c
)
target_include_directories(lib3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(lib3 PROPERTIES
        OUTPUT_NAME "scinput.lib"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib3 PUBLIC c_std_99)