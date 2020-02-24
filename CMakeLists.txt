cmake_minimum_required(VERSION 3.9)
project(HR)

set(CMAKE_CXX_STANDARD 14)


IF(WIN32)
    message("win")
    include_directories(libxl/include_cpp)
    link_directories(libxl/lib64)
ELSEIF(UNIX)
    message("UNIX")
    include_directories(libxl/include_cpp)
    link_directories(libxl/lib)
ELSE()
    include_directories(libxl/include_cpp)
    link_directories(libxl/lib)
    message("OTHER")
ENDIF()


add_executable(HR main.cpp HR_graph.cpp HR_node.cpp LibXLHelper.cpp LibXLHelper.h)
target_link_libraries(HR xl)