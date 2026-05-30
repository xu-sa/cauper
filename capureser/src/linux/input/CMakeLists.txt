
#input

add_library(lib3 STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scinput.c
)
target_include_directories(lib3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(lib3 PROPERTIES
        OUTPUT_NAME "scinput"
        VERSION 0.1
        SOVERSION 1
)
target_compile_features(lib3 PUBLIC c_std_99)

#test app

# add_executable(testapp 
#     ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp
# )
# target_link_libraries(testapp PUBLIC lib3)

# target_include_directories(testapp PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include

# )
# target_compile_features(testapp PUBLIC cxx_std_14)

# set_target_properties(testapp PROPERTIES
#         OUTPUT_NAME "test_input"
# )