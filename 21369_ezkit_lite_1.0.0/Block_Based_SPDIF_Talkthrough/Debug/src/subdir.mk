################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SPORTisr.c \
../src/blockProcess.c \
../src/initPLL_SDRAM.c \
../src/initSPDIF.c \
../src/initSPORT.c \
../src/initSRU.c \
../src/main.c 

SRC_OBJS += \
./src/SPORTisr.doj \
./src/blockProcess.doj \
./src/initPLL_SDRAM.doj \
./src/initSPDIF.doj \
./src/initSPORT.doj \
./src/initSRU.doj \
./src/main.doj 

C_DEPS += \
./src/SPORTisr.d \
./src/blockProcess.d \
./src/initPLL_SDRAM.d \
./src/initSPDIF.d \
./src/initSPORT.d \
./src/initSRU.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/SPORTisr.doj: ../src/SPORTisr.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/SPORTisr.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/blockProcess.doj: ../src/blockProcess.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/blockProcess.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initPLL_SDRAM.doj: ../src/initPLL_SDRAM.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initPLL_SDRAM.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSPDIF.doj: ../src/initSPDIF.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSPDIF.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSPORT.doj: ../src/initSPORT.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSPORT.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initSRU.doj: ../src/initSRU.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/initSRU.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.doj: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="Block_Based_SPDIF_Talkthrough" -proc ADSP-21369 -flags-compiler --no_wrap_diagnostics -si-revision any -g -D_DEBUG -DCORE0 @includes-6bd30a51bbbee1341d12d12dc5fed90f.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -gnu-style-dependencies -MD -Mo "src/main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


