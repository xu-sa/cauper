
#capture

add_library(lib1 STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppwcap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppwcallback1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppwcallback2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppwportal.c   
)

target_include_directories(lib1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PIPEWIRE_INC}
    ${PORTAL_INC}
    ${GLIB_INC}
)

set_target_properties(lib1 PROPERTIES
        OUTPUT_NAME "sccap"
        VERSION 0.1
        SOVERSION 1
)

target_compile_features(lib1 PUBLIC c_std_99)
