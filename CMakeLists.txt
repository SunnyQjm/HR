cmake_minimum_required(VERSION 3.9)
project(HR)

set(CMAKE_CXX_STANDARD 14)


IF(WIN32)
    message("win")
ELSEIF(UNIX)
    message("UNIX")
    include_directories(libxl/include_cpp)
    link_directories(libxl/lib64)
ELSE()
    include_directories(libxl/include_cpp)
    link_directories(libxl/lib64)
    message("OTHER")
ENDIF()


add_executable(HR main.cpp HR_graph.cpp HR_node.cpp LibXLHelper.cpp LibXLHelper.h)
target_link_libraries(HR xl)