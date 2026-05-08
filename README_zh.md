# STM32 Toolkit — Linux 嵌入式开发工具集

[English](README.md)

一键在 Linux 上创建、编译、烧录 STM32 项目。支持裸机（无库）、SPL（标准外设库）和 HAL（硬件抽象层）三种模式。

```bash
# 1. 克隆
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 2. 一键安装（自动检测系统，装依赖，配权限）
bash setup.sh

# 3. 重新加载配置
source ~/.profile

# 4. 新建项目
stm32make new led-blink F103C8

# 5. 编译
cd led-blink && stm32make build

# 6. 插上 ST-Link，烧录
stm32make flash
```

## 快速开始

```bash
# ── 裸机模式（无库，最小依赖）──
stm32make new myproj F103RCT6

# ── SPL 模式（标准外设库）──
stm32make new myproj F103RCT6 --spl

# ── HAL 模式（硬件抽象层）──
stm32make new myproj F103RCT6 --hal

# ── 编译 & 烧录 ──
cd myproj
stm32make build          # 一键编译
stm32make flash          # 自动检测 ST-Link 或串口 ISP，烧录
```

## 功能特性

| 命令 | 说明 |
|:-----|:------|
| `stm32make new <项目名> <芯片>` | 生成项目（裸机，默认） |
| `stm32make new <项目名> <芯片> --spl` | 生成项目（带 SPL 库） |
| `stm32make new <项目名> <芯片> --hal` | 生成项目（带 HAL 库，STM32CubeF1） |
| `stm32make build` | 从项目目录一键编译 |
| `stm32make flash` | 自动检测 ST-Link 或串口 ISP，烧录 |
| `stm32make flash --use-reset` | 烧录 + 硬件复位（需接 NRST 线） |
| `stm32make flash build/app.bin` | 烧录指定 .bin 文件 |
| `stm32make list` | 列出支持的芯片模板 |
| `stm32make doctor` | 诊断开发环境 |
| `stm32make update` | 从 GitHub 拉取最新版 |

语言自动切换：根据 `$LANG` 自动显示中文或英文。

## 支持芯片

| 芯片 | 命令 | Flash | RAM | 常见板子 |
|:-----|:-----|:------|:----|:--------|
| STM32F103C8 | `stm32make new xxx F103C8` | 64K | 20K | Blue Pill, 最小系统板 |
| STM32F103RCT6 | `stm32make new xxx F103RCT6` | 256K | 48K | MiniSTM32, 各种开发板 |

新芯片可以通过创建新的模板目录添加（见「添加新芯片」）。

## 生成的项目结构

**SPL / 裸机模式：**

```
myproj/
├── CMakeLists.txt          ← 编译配置（mcpu, 编译选项, 源文件）
├── link.ld                 ← 链接脚本（Flash/RAM 布局, .data/.bss）
├── inc/
│   ├── board.h             ← **板级引脚 + 时钟配置（改这里！）**
│   └── stm32f10x_conf.h    ← SPL 模块选择（默认全开）
├── src/
│   ├── startup.c           ← 完整向量表 + .data/.bss 初始化
│   └── main.c              ← 应用入口（含 USER CODE 区域和时钟注释）
├── Drivers/
│   ├── CMSIS/              ← ARM Cortex-M 内核头文件 + system_stm32f10x.c
│   └── SPL/                ← 标准外设库（--spl 时导入）
└── build/
    └── myproj.bin          ← 编译产物
```

**HAL 模式：**

```
myproj/
├── CMakeLists.txt
├── link.ld
├── inc/
│   └── stm32f1xx_hal_conf.h
├── src/
│   ├── main.c              ← 应用（完整 HAL 示例：LED + 按键）
│   ├── startup.c           ← 完整向量表 + .data/.bss 初始化
│   └── system_stm32f10x.c  ← 时钟配置（SystemInit → 72MHz PLL）
├── Drivers/
│   ├── CMSIS/              ← CMSIS_F1 头文件
│   └── STM32F1_HAL_Driver/ ← STM32CubeF1 HAL 驱动库
└── build/
```

### 模板特性

生成的项目包含：

- **`startup.c`** — 完整向量表（HD 82 项 / MD 51 项）。所有中断默认指向弱符号 `Default_Handler`（死循环）。需要哪个中断，直接定义对应的 `IRQHandler` 即可自动覆盖。
- **`board.h`** — 所有硬件引脚配置集中在一个文件。换板子只需改这一个文件。内含时钟配置指引（晶振频率、PLL 设置、HSI 回退方案）。
- **`USER CODE` 区域** — `main.c` 设有 `USER CODE BEGIN/END` 标记。USART/printf 代码块用 `#if 0/#endif` 开关——改为 `#if 1` 即可启用。
- **不再硬编码 SP** — 栈顶从链接脚本自动计算，换芯片无需手动改地址。
- **SystemInit() 自动调用** — 项目默认跑 72MHz（8MHz HSE → PLL），而非 8MHz HSI。
- **.data/.bss 段** — 全局变量和 printf 开箱即用。
- **CMSIS 集成** — `system_stm32f10x.c` 自动编译。

## 环境诊断

```bash
stm32make doctor
```

检查以下项目：
- arm-none-eabi-gcc / cmake / st-flash / stm32flash / python3
- ST-Link udev 规则
- 串口权限（dialout / uucp 组）
- 芯片模板是否存在
- 操作系统信息

## 更新

```bash
stm32make update
```

自动执行 `git pull`。`~/bin/stm32make` 是仓库版本软链接，拉取后新版立即可用。

## VSCode 集成

如果 VSCode 的集成终端找不到 `stm32make`：

1. **软链接（推荐）：** 安装脚本已创建 `~/bin/stm32make → stm32-toolkit/bin/stm32make`。如果 `~/bin` 已在 PATH 中（Ubuntu 默认），重启 VSCode 即可。

2. **VSCode 设置：** 启用登录 shell 模式：
   ```
   "terminal.integrated.shellArgs.linux": ["-l"]
   ```

3. **手动：** 在终端执行 `source ~/.profile`，或重启 VSCode。

## NRST 接线问题

> `stm32make flash` 默认用 `--connect-under-reset`（**不**加 `--reset`）。
> 因为 ST-Link 的 NRST 引脚在很多 DIY 板子上没有连接。
> 如果 `--reset` 配合未连接的 NRST 使用，芯片烧录后不会启动。
>
> 如果你的 ST-Link 接了 NRST 线，加 `--use-reset` 启用硬件复位：
> ```bash
> stm32make flash --use-reset
> ```

## 架构设计：芯片层 vs 板级

工具集将**芯片级**（CPU 内核、Flash/RAM 大小、启动文件）与**板级**（LED 引脚、串口引脚、按键映射）分离：

```
templates/     ← 芯片级 — 由维护者管理
boards/        ← 板级引脚定义 — 社区贡献
examples/      ← 完整项目 — 任何人都可贡献
```

新建项目时，模板自动生成 `inc/board.h`，内含该芯片最常见封装的默认引脚。如果你的 PCB 引脚不同，只需编辑这一个文件，无需修改应用逻辑。

标准板定义在 `boards/` 目录下供参考：

```bash
# 查看 Blue Pill 引脚定义
cat boards/bluepill.h

# 查看 MiniSTM32 RCT6 引脚定义
cat boards/mini_stm32_rct6.h
```

### 如何移植项目到不同板子

1. 编辑 `inc/board.h` — 修改 LED、串口、按键的宏
2. 重新编译 — `stm32make build`
3. 完成 — `main.c` 不需要任何改动

## 示例项目

仓库 `examples/` 目录下包含了可直接编译的示例项目：

| 项目 | 模式 | 说明 |
|:-----|:-----|:------|
| `TIM_General` | SPL | TIM2 通用定时器，1s LED 闪烁（ISR 计数，主循环轮询） |
| `TIM_Advanced` | SPL | TIM1 高级定时器，双路独立定时（0.5s + 1s 同一颗 LED） |
| `ADC_Temp` | SPL | ADC1 内部温度传感器（通道 16），DMA 循环传输 |
| `ADC_Light` | SPL | ADC1 外部光敏电阻（通道 0, PA0），转换完成中断 |

每个项目都包含 USART1 printf 调试输出，编译烧录方式：

```bash
cd examples/TIM_General
stm32make build && stm32make flash
```

## 串口 ISP 烧录

当未检测到 ST-Link 时，`stm32make flash` 自动回退到串口 ISP 模式：

1. 通过 CH340 DTR/RTS 驱动进入内置 BootLoader
2. 通过 `stm32flash` 经 UART 烧录
3. 需要：BOOT0=高电平（进入 BootLoader），串口连接至 USART1 (PA9/PA10)

## 工具集目录结构

```
stm32-toolkit/
├── setup.sh                   ← 一键安装脚本
├── bin/stm32make              ← 项目生成器 + 烧录工具
├── packs/
│   ├── CMSIS/                 ← ARM 内核头文件 + system_stm32f10x.c
│   ├── CMSIS_F1/              ← HAL 用的 CMSIS（STM32CubeF1）
│   ├── STM32F1_SPL/           ← STM32F1 标准外设库
│   └── STM32F1_HAL_Driver/    ← STM32CubeF1 HAL 驱动
├── templates/
│   ├── F103C8/                ← Blue Pill（裸机）
│   ├── F103C8-spl/            ← Blue Pill（SPL + startup.c）
│   ├── F103RCT6/              ← RCT6（裸机）
│   ├── F103RCT6-spl/          ← RCT6（SPL + startup.c）
│   ├── F103C8-hal/            ← Blue Pill（HAL + startup.c）
│   └── F103RCT6-hal/          ← RCT6（HAL + startup.c）
└── scripts/
```

## 添加新芯片

```bash
cp -r templates/F103RCT6-spl templates/STM32F407VG
# 改 link.ld（Flash/RAM 大小）
# 改 CMakeLists.txt（-mcpu, -D 宏）
```

模板系统是模块化的，在 `templates/` 下创建目录：
- `CHIPNAME/` → 裸机（无库）
- `CHIPNAME-spl/` → SPL 库模式
- `CHIPNAME-hal/` → HAL 模式

每种模式独立，你的芯片支持哪些就建哪些。

## 系统兼容性

| 发行版 | 包管理器 | 状态 | Shell 支持 |
|:-------|:--------|:-----|:----------|
| Ubuntu 20.04+ | apt | ✅ 测试通过 | bash / zsh |
| Debian 11+ | apt | ✅ 理论上 | bash / zsh |
| Fedora 35+ | dnf | ✅ 理论上 | bash / zsh |
| Arch Linux | pacman + yay/paru | ✅ 测试通过 | bash / zsh / fish |
| 其他 | 手动安装 | ⚠️ 需手动装依赖 | — |

## 依赖清单

| 工具 | 用途 | 安装验证 |
|:-----|:-----|:--------|
| `arm-none-eabi-gcc` | ARM 交叉编译器 | `arm-none-eabi-gcc --version` |
| `cmake` | 构建系统 | `cmake --version` |
| `stlink-tools` | ST-Link 烧录 | `st-info --probe` |
| `stm32flash` | 串口 ISP 烧录 | `stm32flash --version` |
| `python3` + `pyserial` | 脚本 + 串口控制 | `python3 -c "import serial"` |

## 已知问题

### 1. VTOR 对齐要求（仅 gcc / Linux）

ARM Cortex-M3 的向量表偏移寄存器 (VTOR) 要求向量表基地址 **512 字节对齐**。Windows 下不是问题（CubeIDE/CubeMX 使用片上 Flash 向量表 `0x08000000`，天然对齐）。

在 gcc 下使用 **HAL + RAM 向量表** 时，必须声明 512 字节对齐：

```c
static uint32_t ram_vector[256] __attribute__((aligned(512)));
```

如果缺少这个属性，链接器可能把数组放到未对齐的地址（如 `0x2000002C`），导致 VTOR 写入被硬件静默忽略。表现为「SysTick 中断破坏 GPIO」——LED 常亮但不闪烁，且没有任何可见错误。

### 2. 必须在 HAL_Init() 前设置 SystemCoreClock

使用内部 HSI 振荡器（8 MHz，复位后默认）时，`.data` 段中的 `SystemCoreClock` 初始值为 `72000000`（为 72 MHz 运行配置）。如果在 `HAL_Init()` 之前没设 `SystemCoreClock = 8000000`，SysTick 重载值会算成 `72000000 / 1000 = 72000`。实际 HCLK 只有 8 MHz，结果是 9 ms 的 tick 周期（不是 1 ms），`HAL_Delay(300)` 会等 2.7 秒而不是 300 ms。

**修复：** 始终在 `HAL_Init()` 前设置 `SystemCoreClock`：

```c
SystemCoreClock = 8000000;
HAL_Init();
```

### 3. SPL 模板包含 CMSIS 源文件

使用 `--spl` 模式时，模板自动编译 `Drivers/CMSIS/*.c`（包含 `system_stm32f10x.c`，提供 `SystemInit()` 函数）。如果遇到 `SystemInit` 未定义的链接错误，请确认模板是最新版。新版本的模板 CMakeLists.txt 已自动处理。

### 4. 旧模板的 .bss 初始化问题（已修复）

旧版 SPL 模板（2026-05-09 之前）的 `main.c` 将 `main()` 直接用作 `Reset_Handler`，跳过了 `.data`/`.bss` 初始化。如果你在使用旧模板生成的项目时遇到 `printf()` 无输出或程序无声崩溃，请：

1. 更新工具包：`stm32make update`
2. 将 `src/startup.c` 复制到旧项目
3. 将 `main.c` 中的向量表从 `(void(*)())main` 改为 `Reset_Handler`，并加入 `.data`/`.bss` 初始化代码

新模板已包含 `startup.c`，无此问题。
