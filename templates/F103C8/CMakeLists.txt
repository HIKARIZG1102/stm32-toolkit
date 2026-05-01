# 项目名占位符 __PROJECT_NAME__ 会在创建时自动替换
cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(__PROJECT_NAME__ C)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DSTM32F10X_MD -O2 -g3 -ffunction-sections -fdata-sections -std=gnu11")
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_SOURCE_DIR}/link.ld -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -nostartfiles")
file(GLOB SRCS "src/*.c")
include_directories(inc)
add_executable(__PROJECT_NAME__.elf ${SRCS})
add_custom_command(TARGET __PROJECT_NAME__.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary __PROJECT_NAME__.elf __PROJECT_NAME__.bin)
