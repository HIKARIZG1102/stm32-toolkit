# STM32 Toolkit — Linux嵌入式开发工具集

## 安装

```bash
# 克隆
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 加入PATH
echo 'export PATH="$PATH:'$(pwd)'/bin"' >> ~/.bashrc
source ~/.bashrc
```

## 使用

```bash
# 新建项目
stm32make new led-blink F103C8

# 查看支持的芯片
stm32make list
```

## 当前支持芯片

| 芯片 | 内核 | Flash | RAM | 模板路径 |
|:----|:----|:------|:----|:---------|
| STM32F103C8 | Cortex-M3 | 64K | 20K | templates/F103C8 |
| STM32F103RCT6 | Cortex-M3 | 256K | 48K | templates/F103RCT6 |

## 目录结构

```
stm32-toolkit/
├── bin/
│   └── stm32make          ← 项目生成器
├── templates/              ← 芯片模板
│   ├── F103C8/
│   │   ├── CMakeLists.txt
│   │   ├── link.ld
│   │   └── src/main.c
│   └── F103RCT6/
│       ├── CMakeLists.txt
│       ├── link.ld
│       └── src/main.c
└── scripts/                ← 实用脚本(陆续添加)
```

## 烧录方式

**ST-Link:**
```bash
st-flash --reset write build/project.bin 0x08000000
```

**串口ISP (CH340):**
```bash
stm32flash -b 115200 -w build/project.bin -g 0x0 /dev/ttyUSB0
```

## 依赖安装

```bash
sudo apt install gcc-arm-none-eabi cmake stlink-tools stm32flash
```
