################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../render/GeometryExtractorVisitor.cpp \
../render/Ground.cpp \
../render/Selection.cpp \
../render/TextureRepeaterVisitor.cpp 

OBJS += \
./render/GeometryExtractorVisitor.o \
./render/Ground.o \
./render/Selection.o \
./render/TextureRepeaterVisitor.o 

CPP_DEPS += \
./render/GeometryExtractorVisitor.d \
./render/Ground.d \
./render/Selection.d \
./render/TextureRepeaterVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
render/%.o: ../render/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -Wshadow -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


