cmake_minimum_required(VERSION 3.10)
project(capureclt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(PLATFORM_ANDROID OR ANDROID)
    message(STATUS "Building for Android (${ANDROID_ABI})")
    set(PLATFORM_ANDROID TRUE)
    set(PLATFORM_LINUX FALSE)
    set(FFMPEG_INC "${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64-v8a/ffmpeg/include")
    set(LINK_LIB 
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64-v8a/ffmpeg/lib/libavcodec.a 
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64-v8a/ffmpeg/lib/libavutil.a 
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64-v8a/ffmpeg/lib/libswscale.a     
        m z log android
    )
    
else()
    message(STATUS "Building for Linux")
    set(PLATFORM_ANDROID FALSE)
    set(PLATFORM_LINUX TRUE)
    set(FFMPEG_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avutil/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avcodec/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/swscale/include
    )
    set(LINK_LIB
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avcodec/lib/libavcodec.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avutil/lib/libavutil.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/swscale/lib/libswscale.so
    )

endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/decoder) #lib1 static
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/network) #lib3 static
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/client)  #libA shared, link lib1/2/3 and AVCODEC_LIB
if(PLATFORM_LINUX)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)        #testapp
endif()