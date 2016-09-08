################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../base/src/EpollWatch.cpp \
../base/src/IOBufferPtr.cpp \
../base/src/Log.cpp \
../base/src/MsgDeque.cpp \
../base/src/MsgTime.cpp \
../base/src/Mutex.cpp \
../base/src/Socket.cpp \
../base/src/TMap.cpp \
../base/src/ThreadPool.cpp \
../base/src/ThreadTask.cpp \
../base/src/Timeout.cpp \
../base/src/commfunc.cpp 

OBJS += \
./base/src/EpollWatch.o \
./base/src/IOBufferPtr.o \
./base/src/Log.o \
./base/src/MsgDeque.o \
./base/src/MsgTime.o \
./base/src/Mutex.o \
./base/src/Socket.o \
./base/src/TMap.o \
./base/src/ThreadPool.o \
./base/src/ThreadTask.o \
./base/src/Timeout.o \
./base/src/commfunc.o 

CPP_DEPS += \
./base/src/EpollWatch.d \
./base/src/IOBufferPtr.d \
./base/src/Log.d \
./base/src/MsgDeque.d \
./base/src/MsgTime.d \
./base/src/Mutex.d \
./base/src/Socket.d \
./base/src/TMap.d \
./base/src/ThreadPool.d \
./base/src/ThreadTask.d \
./base/src/Timeout.d \
./base/src/commfunc.d 


# Each subdirectory must supply rules for building sources it contributes
base/src/%.o: ../base/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


