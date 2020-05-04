cmake_minimum_required(VERSION 3.5)

project(Examples C CXX)
Set(This examples)
set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    message("Cmake system : ${CMAKE_SYSTEM}")
    message("Cmake System name : ${CMAKE_SYSTEM_NAME}")
    message("Cmake System version : ${CMAKE_SYSTEM_VERSION}")
    message("Cmake System processor : ${CMAKE_SYSTEM_PROCESSOR}")
    message("Cmake Host System name : ${CMAKE_HOST_SYSTEM_NAME}")
    message("Parent Directory: ${PARENT_DIRECTORY}")
elseif (WIN32)
    message("This is a Windows system")
endif()

set (Headers
    spi/spi.h
    spi/var.h
)

set(Sources
    spi/spi.c
    test_spi.c
)

add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} -lpthread)
