cmake_minimum_required (VERSION 2.8)
project (cyberpvnk)

set (OUTPUT "cyberpvnk")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories ("include")

add_executable (${OUTPUT}
	src/state/Game.cpp
	src/Engine.cpp
	src/main.cpp
)

target_link_libraries (${OUTPUT}
	sfml-graphics
	sfml-window
	sfml-system
)
