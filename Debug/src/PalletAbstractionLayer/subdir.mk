################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PalletAbstractionLayer/Pallet.cpp \
../src/PalletAbstractionLayer/RFIDAnswerProcess.cpp \
../src/PalletAbstractionLayer/RFIDPetitionProcess.cpp \
../src/PalletAbstractionLayer/VirtualPalletOperations.cpp 

OBJS += \
./src/PalletAbstractionLayer/Pallet.o \
./src/PalletAbstractionLayer/RFIDAnswerProcess.o \
./src/PalletAbstractionLayer/RFIDPetitionProcess.o \
./src/PalletAbstractionLayer/VirtualPalletOperations.o 

CPP_DEPS += \
./src/PalletAbstractionLayer/Pallet.d \
./src/PalletAbstractionLayer/RFIDAnswerProcess.d \
./src/PalletAbstractionLayer/RFIDPetitionProcess.d \
./src/PalletAbstractionLayer/VirtualPalletOperations.d 


# Each subdirectory must supply rules for building sources it contributes
src/PalletAbstractionLayer/%.o: ../src/PalletAbstractionLayer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/andres/git/ControlMachine14/src/connexion" -I"/home/andres/git/ControlMachine14/src" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


