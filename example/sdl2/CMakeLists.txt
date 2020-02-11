project(example)

find_package(SDL2 REQUIRED)

set(EXAMPLE_SOURCES 
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${LIBMBMETEOR_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})

add_executable(example ${EXAMPLE_SOURCES})
target_link_libraries(example libmbmeteor BeeARM ${SDL2_LIBRARIES})
