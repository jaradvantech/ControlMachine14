################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Algorithm.cpp \
../src/Prueba1.cpp \
../src/RoboticArmInfo.cpp \
../src/StorageInterface.cpp 

OBJS += \
./src/Algorithm.o \
./src/Prueba1.o \
./src/RoboticArmInfo.o \
./src/StorageInterface.o 

CPP_DEPS += \
./src/Algorithm.d \
./src/Prueba1.d \
./src/RoboticArmInfo.d \
./src/StorageInterface.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/baseconfig/git/ControlMachine14/src/connexion" -I"/home/baseconfig/git/ControlMachine14/src" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


