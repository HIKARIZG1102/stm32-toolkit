cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "Cross-compiler for ARM")
project(__PROJECT_NAME__ C)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -O2 -g3 -ffunction-sections -fdata-sections -std=gnu11")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -nostartfiles")

# CMSIS + SPL 头文件
include_directories(inc Drivers/CMSIS Drivers/SPL)

# SPL 源文件（只启用需要的模块，省编译时间）
file(GLOB SPL_SRC "Drivers/SPL/stm32f10x_gpio.c" "Drivers/SPL/stm32f10x_rcc.c" "Drivers/SPL/stm32f10x_usart.c" "Drivers/SPL/stm32f10x_flash.c" "Drivers/SPL/stm32f10x_exti.c")

# 用户源文件
file(GLOB USER_SRC "src/*.c")

add_executable(__PROJECT_NAME__.elf ${SPL_SRC} ${USER_SRC})
add_custom_command(TARGET __PROJECT_NAME__.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary __PROJECT_NAME__.elf __PROJECT_NAME__.bin)
