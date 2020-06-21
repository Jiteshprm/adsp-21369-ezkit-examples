################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SPORTisr.c \
../src/init1835viaSPI.c \
../src/initPLL_SDRAM.c \
../src/initSPDIF.c \
../src/initSPORT.c \
../src/initSRC.c \
../src/initSRU.c \
../src/main.c 

SRC_OBJS += \
./src/SPORTisr.doj \
./src/init1835viaSPI.doj \
./src/initPLL_SDRAM.doj \
./src/initSPDIF.doj \
./src/initSPORT.doj \
./src/initSRC.doj \
./src/initSRU.doj \
./src/main.doj 

C_DEPS += \
./src/SPORTisr.d \
./src/init1835viaSPI.d \
./src/initPLL_SDRAM.d \
./src/initSPDIF.d \
./src/initSPORT.d \
./src/initSRC.d \
./src/initSRU.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/SPORTisr.doj: ../src/SPORTisr.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/SPORTisr.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/init1835viaSPI.doj: ../src/init1835viaSPI.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/init1835viaSPI.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initPLL_SDRAM.doj: ../src/initPLL_SDRAM.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initPLL_SDRAM.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSPDIF.doj: ../src/initSPDIF.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSPDIF.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSPORT.doj: ../src/initSPORT.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSPORT.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSRC.doj: ../src/initSRC.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSRC.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSRU.doj: ../src/initSRU.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSRU.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.doj: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Digital_In_Analog_Out_With_SRC" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-9e493c494c6cfc805cb8ccb9b74a0828.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


