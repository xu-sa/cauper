add_executable(app
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    
)
target_link_directories(app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/server/
)
target_link_libraries(app PUBLIC libB)

target_compile_features(libB PUBLIC cxx_std_14)