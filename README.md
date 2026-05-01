# STM32 Toolkit — Linux嵌入式开发工具集

一键安装，支持 Ubuntu / Debian / Fedora / Arch (含 AUR)。
自动检测 Shell (bash/zsh/fish) 和串口组名。

## 快速开始

```bash
# 1. 克隆
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 2. 一键安装（自动检测系统，装依赖，配权限）
bash setup.sh

# 3. 重新加载配置
source ~/.bashrc

# 4. 新建一个项目
stm32make new led-blink F103C8

# 5. 编译
cd led-blink
cmake -B build && cmake --build build

# 6. 插上ST-Link，烧录
stm32make flash
```

## 分步说明

### 第一步：克隆 + 安装

```bash
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit
bash setup.sh
```

`setup.sh` 会自动完成：
- 检测你的 Linux 发行版（Ubuntu/Debian/Fedora/Arch）
- 安装所有依赖（arm-none-eabi-gcc, cmake, stlink-tools, stm32flash, python3）
- 配置 ST-Link udev 规则（免sudo使用ST-Link）
- 添加 PATH 到 ~/.bashrc
- 将你加入 dialout 组（串口权限）

### 第二步：新建项目

```bash
stm32make new 项目名 芯片型号
```

支持芯片：

| 芯片 | 命令 | Flash | RAM | 常见板子 |
|:----|:-----|:------|:----|:--------|
| STM32F103C8 | `stm32make new xxx F103C8` | 64K | 20K | Blue Pill, 最小系统板 |
| STM32F103RCT6 | `stm32make new xxx F103RCT6` | 256K | 48K | 各种开发板 |

生成的结构：
```
项目名/
├── CMakeLists.txt    ← 编译配置（芯片参数已自动填好）
├── link.ld           ← 链接脚本（Flash/RAM大小已匹配）
├── inc/              ← 放头文件
├── src/main.c        ← 代码入口（向量表已写好，直接写逻辑）
└── build/            ← 编译产物
```

### 第三步：编译

```bash
cd 项目名
cmake -B build        # 配置（首次）
cmake --build build   # 编译（之后只跑这个）
```

> **注意**: `setup.sh` 已自动将 `stm32make` 加入 PATH。
> 如果使用 Fish shell, 运行 `source ~/.config/fish/config.fish` 或开新终端。
> 不要 `source ~/.bashrc` — Fish 不兼容 Bash 语法。

编译产物：
- `build/项目名.elf` — 调试用
- `build/项目名.bin` — 烧录用

### 第四步：烧录

**插上ST-Link或USB转TTL：**

```bash
# 自动检测设备并烧录
stm32make flash

# 或指定文件
stm32make flash build/led-blink.bin
```

`stm32make flash` 的自动检测逻辑：
1. 优先检测 ST-Link → 直接烧
2. 没有 ST-Link → 扫描 /dev/ttyUSB* 和 /dev/ttyACM*
3. 只有一个串口 → 直接用
4. 多个串口 → 让你选
5. 没设备 → 报错引导你排查

### 完整工作流（日常开发）

```bash
# 改代码 → 编译 → 烧录 → 看效果
code src/main.c                       # 改代码
cmake --build build                   # 编译
st-flash --reset write build/*.bin 0x08000000  # 或: stm32make flash
picocom -b 115200 /dev/ttyUSB0        # 看串口输出
```

## stm32make 命令大全

```bash
stm32make new led-blink F103C8    # 新建C8T6项目
stm32make new robot F103RCT6      # 新建RCT6项目
stm32make list                    # 查看支持芯片
stm32make flash                   # 自动检测设备并烧录
stm32make flash build/app.bin     # 烧录指定文件
```

## 烧录方式对比

| 方式 | 设备 | 接线数 | 速度 | 调试 | 应用场景 |
|:----|:----|:------|:----|:----|:--------|
| ST-Link | ST-Link/V2 | 4线 (SWD) | 快 | ✅ 支持 | 日常开发 |
| 串口ISP | CH340/CP2102 | 5线 (UART) | 慢 | ❌ | 部署调试 |

## 项目目录结构

```
stm32-toolkit/
├── setup.sh                   ← 一键安装脚本
├── bin/stm32make              ← 项目生成器 + 烧录工具
├── templates/                 ← 芯片模板（可自行添加）
│   ├── F103C8/                ← Blue Pill
│   └── F103RCT6/              ← RCT6开发板
└── scripts/                   ← 工具脚本
```

## 添加新芯片

```bash
# 复制模板，改三个文件
cp -r templates/F103C8 templates/STM32F407VG
# 1. 改 link.ld 的 Flash/RAM 大小
# 2. 改 CMakeLists.txt 的 -D 宏和 -mcpu
# 3. 改 src/main.c 的 SP 栈顶地址
```

## 系统兼容性

|| 发行版 | 包管理器 | 状态 | Shell支持 |
||:------|:--------|:----|:---------|
|| Ubuntu 20.04+ | apt | ✅ 测试通过 | bash / zsh |
|| Debian 11+ | apt | ✅ 理论上 | bash / zsh |
|| Fedora 35+ | dnf | ✅ 理论上 | bash / zsh |
|| Arch Linux | pacman + yay/paru | ✅ 测试通过 | bash / zsh / fish |
| 其他 | 手动安装 | ⚠️ 需手动装依赖 |

## 依赖清单

| 工具 | 用途 | 安装验证 |
|:----|:-----|:--------|
| `arm-none-eabi-gcc` | ARM交叉编译器 | `arm-none-eabi-gcc --version` |
| `cmake` | 构建系统 | `cmake --version` |
| `stlink-tools` | ST-Link烧录 | `st-info --probe` |
| `stm32flash` | 串口ISP烧录 | `stm32flash --version` |
| `python3` | 烧录脚本 | `python3 --version` |
