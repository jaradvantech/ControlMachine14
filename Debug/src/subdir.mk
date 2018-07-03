################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Algorithm.cpp \
../src/ConfigParser.cpp \
../src/PLC_communications.cpp \
../src/Prueba1.cpp \
../src/RoboticArmInfo.cpp \
../src/StorageInterface.cpp \
../src/SynchronizationPoints.cpp 

OBJS += \
./src/Algorithm.o \
./src/ConfigParser.o \
./src/PLC_communications.o \
./src/Prueba1.o \
./src/RoboticArmInfo.o \
./src/StorageInterface.o \
./src/SynchronizationPoints.o 

CPP_DEPS += \
./src/Algorithm.d \
./src/ConfigParser.d \
./src/PLC_communications.d \
./src/Prueba1.d \
./src/RoboticArmInfo.d \
./src/StorageInterface.d \
./src/SynchronizationPoints.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/baseconfig/git/ControlMachine14/src/connexion" -I"/home/baseconfig/git/ControlMachine14/src" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


