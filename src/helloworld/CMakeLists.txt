add_executable(helloworld helloworld.cpp)
target_link_libraries(helloworld ${OpenCL_LIBRARY})

configure_file(helloworld.cl ${CMAKE_CURRENT_BINARY_DIR}/helloworld.cl COPYONLY)