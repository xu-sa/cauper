add_executable(testapp1
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(testapp1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/client
)

target_link_libraries(testapp1 PUBLIC
    libA
)

set_target_properties(testapp1 PROPERTIES
    OUTPUT_NAME "TestA"
)