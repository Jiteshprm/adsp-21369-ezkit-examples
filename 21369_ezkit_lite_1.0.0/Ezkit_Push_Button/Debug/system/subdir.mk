################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/adi_initialize.c 

SRC_OBJS += \
./system/adi_initialize.doj 

C_DEPS += \
./system/adi_initialize.d 


# Each subdirectory must supply rules for building sources it contributes
system/adi_initialize.doj: ../system/adi_initialize.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Ezkit_Push_Button" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-368683f63649f5f13372872dfb870ad4.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "system/adi_initialize.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


