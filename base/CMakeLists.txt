SET(base_SRC
	src/Log.cpp
	src/commfunc.cpp
	src/TMap.cpp
	src/Mutex.cpp
	src/ThreadPool.cpp
	src/MsgTime.cpp
	src/Timeout.cpp
	src/MsgDeque.cpp
	src/Socket.cpp
	src/EpollWatch.cpp
	src/IOBufferPtr.cpp
	src/ThreadTask.cpp)
	
ADD_LIBRARY(base SHARED ${base_SRC})

set(CMAKE_CXX_COMPILER "g++")

SET(HEADERS
	src/Log.h
	src/TMap.h
	src/logdef.h
	src/datatype.h
	src/commfunc.h
	src/Mutex.h
	src/ThreadPool.h
	src/ThreadDefine.h
	src/MsgTime.h
	src/Timeout.h
	src/MsgDeque.h
	src/Socket.h
	src/EpollWatch.h
	src/IOBufferPtr.h
	src/ThreadTask.h)	
