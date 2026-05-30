
if(PLATFORM_ANDROID)
add_library(libA SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_api.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_utils.cpp
)
else()
add_library(libA SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_utils.cpp
)
endif()

target_include_directories(libA PUBLIC
    ${FFMPEG_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/../encoder/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../network/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_options(libA PRIVATE "-Wl,-Bsymbolic")

target_link_libraries(libA PUBLIC
    lib1 
    lib3
    ${LINK_LIB} 
)
set_target_properties(libA PROPERTIES
        OUTPUT_NAME "cauper"
)
 