
cmake_minimum_required(VERSION 3.10)
project(capureser)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(WIN32)
#windows toolchain
    set(CMAKE_SYSTEM_NAME Windows)
    set(LINK_LIB
        ws2_32    
    )

    set(INC_MODULE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/capture/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/encoder/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/input/include
    )
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/windows/capture)#lib1
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/windows/encoder)#lib2
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/windows/input)#lib3

else()
#native linux
    set(CMAKE_SYSTEM_NAME Linux)

    set(AVCODEC_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avcodec)
    set(AVUTIL_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avutil)
    set(SWSCALE_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/swscale)
    set(PIPEWIRE_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/pipewire/include)
    set(PORTAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/libportal/include)
    set(GLIB_INC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/glib/include)

    set(AVCODEC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avcodec/lib/libavcodec.so)
    set(AVUTIL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/avutil/lib/libavutil.so)
    set(SWSCALE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/swscale/lib/libswscale.so)
    set(PIPEWIRE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/pipewire/lib/libpipewire-0.3.so)
    set(PORTAL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/libportal/lib/libportal.so)
    set(GLIB_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/glib/lib/libglib-2.0.so)
    
    set(LINK_LIB#all Third party libs for linux
        ${AVCODEC_LIB}
        ${AVUTIL_LIB}
        ${SWSCALE_LIB}
        ${PIPEWIRE_LIB}
        ${PORTAL_LIB}
        ${GLIB_LIB}
    )
    set(INC_MODULE#module headers for linux
        ${CMAKE_CURRENT_SOURCE_DIR}/src/linux/capture/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/linux/encoder/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/linux/input/include
    )
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/linux/capture)#lib1
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/linux/encoder)#lib2
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/linux/input)#lib3

endif()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/public/network)#lib4
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/public/server)#libB
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)#testapp
