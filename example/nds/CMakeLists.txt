project(nds)

find_package(SDL2 REQUIRED)

set(EXAMPLE_SOURCES 
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${LIBMBMETEOR_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})

add_executable(nds ${EXAMPLE_SOURCES})
target_link_libraries(nds libmbmeteor BeeARM ${SDL2_LIBRARIES})
