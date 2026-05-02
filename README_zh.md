# STM32 Toolkit — Linux嵌入式开发工具集

[English](README.md)

一键在 Linux 上创建、编译、烧录 STM32 项目。支持 Ubuntu / Debian / Fedora / Arch。

```bash
# 1. 克隆
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 2. 一键安装（自动检测系统，装依赖，配权限）
bash setup.sh

# 3. 重新加载配置
source ~/.bashrc

# 4. 新建项目
stm32make new led-blink F103C8

# 5. 编译
cd led-blink
cmake -B build && cmake --build build

# 6. 插上ST-Link，烧录
stm32make flash
```

## 功能特性

- **`stm32make new <项目名> <芯片型号>`** — 生成完整项目（向量表、链接脚本、CMakeLists.txt）
- **`stm32make flash`** — 自动检测 ST-Link 或串口 ISP，一键烧录
- **`stm32make doctor`** — 诊断开发环境（编译器、工具、权限）
- **`stm32make update`** — 从 GitHub 拉取最新版
- **`stm32make list`** — 列出支持的芯片模板
- **SPL 模式** (`--spl`) — 可选导入 STM32 标准外设库
- **语言自动切换** — 根据 `$LANG` 自动显示中文或英文

## 支持芯片

| 芯片 | 命令 | Flash | RAM | 常见板子 |
|:----|:-----|:------|:----|:--------|
| STM32F103C8 | `stm32make new xxx F103C8` | 64K | 20K | Blue Pill, 最小系统板 |
| STM32F103RCT6 | `stm32make new xxx F103RCT6` | 256K | 48K | 各种开发板 |

## 命令大全

```bash
stm32make new led-blink F103C8           # 新建C8T6项目（裸机）
stm32make new led-blink F103C8 --spl     # 新建C8T6项目（带SPL库）
stm32make new robot F103RCT6             # 新建RCT6项目
stm32make list                           # 查看支持芯片
stm32make flash                          # 自动检测设备并烧录
stm32make flash build/app.bin            # 烧录指定文件
stm32make flash --use-reset              # 烧录+硬件复位(NRST需接线)
stm32make doctor                         # 诊断开发环境
stm32make update                         # 从GitHub获取最新版
```

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

自动执行 `git pull`。因为 `setup.sh` 已将 `~/stm32-toolkit/bin/` 加入 PATH，拉取后新版立即可用。

## NRST 接线问题

> `stm32make flash` 默认用 `--connect-under-reset`（**不**加 `--reset`）。
> 因为 ST-Link 的 NRST 引脚在很多 DIY 板子上没有连接。
> 如果 `--reset` 配合未连接的 NRST 使用，芯片烧录后不会启动。
>
> 如果你的 ST-Link 接了 NRST 线，加 `--use-reset` 启用硬件复位：
> ```bash
> stm32make flash --use-reset
> ```

## 项目目录结构

```
stm32-toolkit/
├── setup.sh                   ← 一键安装脚本
├── bin/stm32make              ← 项目生成器 + 烧录工具
├── packs/
│   ├── CMSIS/                 ← ARM内核头文件
│   └── STM32F1_SPL/           ← STM32F1标准外设库 (SLA0044)
├── templates/                 ← 芯片模板
│   ├── F103C8/                ← Blue Pill (裸机)
│   ├── F103C8-spl/            ← Blue Pill (SPL)
│   ├── F103RCT6/              ← RCT6 (裸机)
│   └── F103RCT6-spl/          ← RCT6 (SPL)
└── scripts/
```

生成的项目结构：

```
项目名/
├── CMakeLists.txt
├── link.ld
├── inc/
├── src/main.c
└── build/
```

## 添加新芯片

```bash
cp -r templates/F103C8 templates/STM32F407VG
# 1. 改 link.ld 的 Flash/RAM 大小
# 2. 改 CMakeLists.txt 的 -mcpu 和 -D 宏
# 3. 改 src/main.c 的 SP 栈顶地址
```

## 系统兼容性

| 发行版 | 包管理器 | 状态 | Shell支持 |
|:------|:--------|:----|:---------|
| Ubuntu 20.04+ | apt | ✅ 测试通过 | bash / zsh |
| Debian 11+ | apt | ✅ 理论上 | bash / zsh |
| Fedora 35+ | dnf | ✅ 理论上 | bash / zsh |
| Arch Linux | pacman + yay/paru | ✅ 测试通过 | bash / zsh / fish |
| 其他 | 手动安装 | ⚠️ 需手动装依赖 | — |

## 依赖清单

| 工具 | 用途 | 安装验证 |
|:----|:-----|:--------|
| `arm-none-eabi-gcc` | ARM交叉编译器 | `arm-none-eabi-gcc --version` |
| `cmake` | 构建系统 | `cmake --version` |
| `stlink-tools` | ST-Link烧录 | `st-info --probe` |
| `stm32flash` | 串口ISP烧录 | `stm32flash --version` |
| `python3` | 脚本 | `python3 --version` |

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
