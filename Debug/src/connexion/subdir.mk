################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connexion/TCPClientLibrary.cpp \
../src/connexion/TCPServerLibrary.cpp \
../src/connexion/connexionDisplay.cpp \
../src/connexion/connexionPLC.cpp \
../src/connexion/connexionPLCdatahandler.cpp \
../src/connexion/connexionRFID.cpp 

OBJS += \
./src/connexion/TCPClientLibrary.o \
./src/connexion/TCPServerLibrary.o \
./src/connexion/connexionDisplay.o \
./src/connexion/connexionPLC.o \
./src/connexion/connexionPLCdatahandler.o \
./src/connexion/connexionRFID.o 

CPP_DEPS += \
./src/connexion/TCPClientLibrary.d \
./src/connexion/TCPServerLibrary.d \
./src/connexion/connexionDisplay.d \
./src/connexion/connexionPLC.d \
./src/connexion/connexionPLCdatahandler.d \
./src/connexion/connexionRFID.d 


# Each subdirectory must supply rules for building sources it contributes
src/connexion/%.o: ../src/connexion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/baseconfig/eclipse-workspace/ControlMachine/src/connexion" -I"/home/baseconfig/eclipse-workspace/ControlMachine/src" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


