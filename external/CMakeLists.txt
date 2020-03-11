add_definitions(-DBEEARM_ENABLE_THUMB)

set(BEEARM_HEADERS
	arm/BeeARM/BeeARM/beearm.h
	arm/BeeARM/BeeARM/beearm_tables.h
	arm/BeeARM/BeeARM/beearm_interpreterdefines.h
	arm/BeeARM/BeeARM/beearm_interpreterarm.h
	arm/BeeARM/BeeARM/beearm_interpreterthumb.h)

set(BEEARM_SOURCES
	arm/BeeARM/BeeARM/beearm.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(BeeARM STATIC ${BEEARM_SOURCES} ${BEEARM_HEADERS})

target_include_directories(BeeARM PUBLIC
	arm/BeeARM/)
