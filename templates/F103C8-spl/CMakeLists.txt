cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "Cross-compiler for ARM")
project(__PROJECT_NAME__ C)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -O2 -g3 -ffunction-sections -fdata-sections -std=gnu11")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -nostartfiles -u _printf_float")

include_directories(inc Drivers/CMSIS Drivers/SPL)

# SPL + CMSIS 源文件（system_stm32f10x.c 提供 SystemInit）
file(GLOB SPL_SRC "Drivers/SPL/*.c" "Drivers/CMSIS/*.c")

file(GLOB USER_SRC "src/*.c")

add_executable(__PROJECT_NAME__.elf ${SPL_SRC} ${USER_SRC})
target_link_libraries(__PROJECT_NAME__.elf m)
add_custom_command(TARGET __PROJECT_NAME__.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary __PROJECT_NAME__.elf __PROJECT_NAME__.bin)
