################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../base/build/CMakeFiles/2.8.11/CompilerIdC/CMakeCCompilerId.c 

OBJS += \
./base/build/CMakeFiles/2.8.11/CompilerIdC/CMakeCCompilerId.o 

C_DEPS += \
./base/build/CMakeFiles/2.8.11/CompilerIdC/CMakeCCompilerId.d 


# Each subdirectory must supply rules for building sources it contributes
base/build/CMakeFiles/2.8.11/CompilerIdC/%.o: ../base/build/CMakeFiles/2.8.11/CompilerIdC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


