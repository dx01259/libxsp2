################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../net/src/AsyncSocket.cpp \
../net/src/HttpService.cpp \
../net/src/InetAddr.cpp \
../net/src/InetTable.cpp \
../net/src/LocalIpList.cpp \
../net/src/SocketEpollTask.cpp \
../net/src/TcpService.cpp \
../net/src/UdpService.cpp 

OBJS += \
./net/src/AsyncSocket.o \
./net/src/HttpService.o \
./net/src/InetAddr.o \
./net/src/InetTable.o \
./net/src/LocalIpList.o \
./net/src/SocketEpollTask.o \
./net/src/TcpService.o \
./net/src/UdpService.o 

CPP_DEPS += \
./net/src/AsyncSocket.d \
./net/src/HttpService.d \
./net/src/InetAddr.d \
./net/src/InetTable.d \
./net/src/LocalIpList.d \
./net/src/SocketEpollTask.d \
./net/src/TcpService.d \
./net/src/UdpService.d 


# Each subdirectory must supply rules for building sources it contributes
net/src/%.o: ../net/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


