
#network
add_library(lib4 STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scudp.c
)
target_include_directories(lib4 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(lib4 PROPERTIES
        OUTPUT_NAME "scnetwork"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib4 PUBLIC c_std_99)
