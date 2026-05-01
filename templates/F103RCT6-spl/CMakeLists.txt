cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "Cross-compiler for ARM")
project(__PROJECT_NAME__ C)
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -O2 -g3 -ffunction-sections -fdata-sections -std=gnu11")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -nostartfiles")

# ── 自动查找 packs 目录（支持项目在 stm32-toolkit 内/外）──
# 优先级: 1. 向上找 ../packs  2. STM32_TOOLKIT 环境变量  3. ~/stm32-toolkit/packs
if(EXISTS "${CMAKE_SOURCE_DIR}/../packs/CMSIS")
  set(PACKS_DIR "${CMAKE_SOURCE_DIR}/../packs")
elseif(DEFINED ENV{STM32_TOOLKIT} AND EXISTS "$ENV{STM32_TOOLKIT}/packs/CMSIS")
  set(PACKS_DIR "$ENV{STM32_TOOLKIT}/packs")
elseif(EXISTS "$ENV{HOME}/stm32-toolkit/packs/CMSIS")
  set(PACKS_DIR "$ENV{HOME}/stm32-toolkit/packs")
else()
  message(FATAL_ERROR "找不到 stm32-toolkit packs 目录！\n"
          "请设置环境变量: export STM32_TOOLKIT=~/stm32-toolkit\n"
          "或把项目放在 stm32-toolkit 目录下")
endif()
message(STATUS "STM32 packs dir: ${PACKS_DIR}")

# CMSIS + SPL 头文件
include_directories(inc ${PACKS_DIR}/CMSIS ${PACKS_DIR}/STM32F1_SPL)

# SPL 源文件（只启用需要的模块，省编译时间）
file(GLOB SPL_SRC
    "${PACKS_DIR}/STM32F1_SPL/stm32f10x_gpio.c"
    "${PACKS_DIR}/STM32F1_SPL/stm32f10x_rcc.c"
    "${PACKS_DIR}/STM32F1_SPL/stm32f10x_usart.c"
    "${PACKS_DIR}/STM32F1_SPL/stm32f10x_flash.c"
    "${PACKS_DIR}/STM32F1_SPL/stm32f10x_exti.c"
    "${PACKS_DIR}/STM32F1_SPL/misc.c"
)

# 用户源文件
file(GLOB USER_SRC "src/*.c")

add_executable(__PROJECT_NAME__.elf ${SPL_SRC} ${USER_SRC})
add_custom_command(TARGET __PROJECT_NAME__.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary __PROJECT_NAME__.elf __PROJECT_NAME__.bin)
