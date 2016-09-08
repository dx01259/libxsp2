################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../net/build/CMakeFiles/2.8.11/CompilerIdCXX/CMakeCXXCompilerId.cpp 

OBJS += \
./net/build/CMakeFiles/2.8.11/CompilerIdCXX/CMakeCXXCompilerId.o 

CPP_DEPS += \
./net/build/CMakeFiles/2.8.11/CompilerIdCXX/CMakeCXXCompilerId.d 


# Each subdirectory must supply rules for building sources it contributes
net/build/CMakeFiles/2.8.11/CompilerIdCXX/%.o: ../net/build/CMakeFiles/2.8.11/CompilerIdCXX/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


