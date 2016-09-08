################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include net/src/subdir.mk
-include net/build/CMakeFiles/net.dir/src/subdir.mk
-include net/build/CMakeFiles/2.8.11/CompilerIdCXX/subdir.mk
-include net/build/CMakeFiles/2.8.11/CompilerIdC/subdir.mk
-include examples/TcpService/subdir.mk
-include cmake/net/CMakeFiles/net.dir/src/subdir.mk
-include cmake/examples/TcpService/CMakeFiles/TcpService.dir/subdir.mk
-include cmake/base/CMakeFiles/base.dir/src/subdir.mk
-include cmake/CMakeFiles/2.8.11/CompilerIdCXX/subdir.mk
-include cmake/CMakeFiles/2.8.11/CompilerIdC/subdir.mk
-include base/src/subdir.mk
-include base/build/CMakeFiles/base.dir/src/subdir.mk
-include base/build/CMakeFiles/2.8.11/CompilerIdCXX/subdir.mk
-include base/build/CMakeFiles/2.8.11/CompilerIdC/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libxsp2

# Tool invocations
libxsp2: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "libxsp2" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) libxsp2
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
