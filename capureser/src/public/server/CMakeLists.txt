add_library(libB SHARED 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server_utils.cpp
    
)

target_include_directories(libB PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${INC_MODULE}#lib123
    ${CMAKE_CURRENT_SOURCE_DIR}/../network/include#lib4    
)
if(LINUX)
target_link_options(libB PRIVATE "-Wl,-Bsymbolic")
endif()
target_link_libraries(libB PUBLIC
    lib1
    lib2
    lib3
    lib4
    ${LINK_LIB}
)
set_target_properties(libB PROPERTIES
        OUTPUT_NAME "capureser"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(libB PUBLIC cxx_std_14)
