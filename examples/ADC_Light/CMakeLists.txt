cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "Cross-compiler for ARM")
project(ADC_Light C)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -O2 -g3 -ffunction-sections -fdata-sections -std=gnu11")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -nostartfiles -u _printf_float")

# CMSIS + SPL 头文件（由 stm32make new --spl 自动复制到 Drivers/）
include_directories(inc Drivers/CMSIS Drivers/SPL)

# SPL 源文件（全部编译）
file(GLOB SPL_SRC "Drivers/SPL/*.c" "Drivers/CMSIS/*.c")

# 用户源文件
file(GLOB USER_SRC "src/*.c")

add_executable(ADC_Light.elf ${SPL_SRC} ${USER_SRC})
target_link_libraries(ADC_Light.elf m)
add_custom_command(TARGET ADC_Light.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ADC_Light.elf ADC_Light.bin)
