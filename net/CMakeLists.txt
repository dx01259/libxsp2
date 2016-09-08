SET(net_SRC
	src/InetAddr.cpp
	src/InetTable.cpp
	src/LocalIpList.cpp
	src/AsyncSocket.cpp
	src/TcpService.cpp
	src/UdpService.cpp
	src/HttpService.cpp
	src/SocketEpollTask.cpp)
	
ADD_LIBRARY(net SHARED ${net_SRC})

set(CMAKE_CXX_COMPILER "g++")
	
SET(HEADERS
	src/InetAddr.h
	src/InetTable.h
	src/LocalIpList.h
	src/AsyncSocket.h
	src/TcpService.h
	src/UdpService.h
	src/HttpService.h
	src/SocketEpollTask.h)