SET(TcpService_SRC
	Main.cpp)

LINK_DIRECTORIES(${xspnet_SOURCE_DIR}/cmake/lib)
INCLUDE_DIRECTORIES()
	
ADD_EXECUTABLE(TcpService ${TcpService_SRC})


TARGET_LINK_LIBRARIES(TcpService net)
TARGET_LINK_LIBRARIES(TcpService base)
TARGET_LINK_LIBRARIES(TcpService pthread)
