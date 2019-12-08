add_library(PID STATIC
	PID_v1.cpp)
target_include_directories(PID PUBLIC
	${CMAKE_CURRENT_LIST_DIR})
