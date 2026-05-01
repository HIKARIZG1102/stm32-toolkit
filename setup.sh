#!/bin/bash
# setup.sh — STM32 Toolkit 一键环境安装
# 支持: Ubuntu, Debian, Fedora, Arch Linux

echo "🔧 STM32 Toolkit 安装脚本"
echo "=========================="
echo ""

# 检测系统
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VER=$VERSION_ID
else
    OS=$(uname -s)
fi

echo "📦 检测到系统: $NAME ($VERSION_ID)"
echo ""

# 安装依赖
install_pkg() {
    case "$OS" in
        ubuntu|debian)
            sudo apt update && sudo apt install -y gcc-arm-none-eabi cmake stlink-tools stm32flash python3 python3-serial
            ;;
        fedora)
            sudo dnf install -y arm-none-eabi-gcc arm-none-eabi-newlib cmake stlink stm32flash python3 pyserial
            ;;
        arch)
            sudo pacman -S --noconfirm arm-none-eabi-gcc arm-none-eabi-newlib cmake stlink stm32flash python python-pyserial
            ;;
        *)
            echo "⚠️  未识别的系统: $OS"
            echo "   请手动安装: arm-none-eabi-gcc, cmake, stlink-tools, stm32flash"
            echo "   然后重新运行此脚本"
            ;;
    esac
}

echo "🚀 即将安装以下工具:"
echo "   - ARM GCC 交叉编译器"
echo "   - CMake 构建系统"
echo "   - ST-Link 烧录工具"
echo "   - stm32flash (串口ISP)"
echo "   - Python3 + pyserial"
echo ""
echo -n "继续安装? [Y/n]: "
read -r CONFIRM
if [ "$CONFIRM" = "n" ] || [ "$CONFIRM" = "N" ]; then
    echo "已取消"
    exit 1
fi

install_pkg

# 下载 CMSIS 头文件（没有包管理器提供时）
if [ ! -f /usr/local/include/core_cm3.h ]; then
    echo "📥 下载 CMSIS 头文件..."
    sudo mkdir -p /usr/local/include
    for f in core_cm3.h cmsis_gcc.h cmsis_compiler.h cmsis_version.h; do
        sudo curl -sL "https://raw.githubusercontent.com/ARM-software/CMSIS_5/master/CMSIS/Core/Include/$f" -o "/usr/local/include/$f"
    done
    echo "✅ CMSIS 头文件已安装到 /usr/local/include/"
fi

# 添加 PATH
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
if ! grep -q "stm32-toolkit" ~/.bashrc 2>/dev/null; then
    echo "export PATH=\"\$PATH:$SCRIPT_DIR/bin\"" >> ~/.bashrc
    echo "✅ 已添加 PATH 到 ~/.bashrc (重新登录或 source ~/.bashrc 生效)"
fi

# 添加 udev 规则（ST-Link 权限）
if [ ! -f /etc/udev/rules.d/49-stlinkv3.rules ]; then
    echo "📥 配置 ST-Link udev 规则..."
    sudo curl -sL "https://raw.githubusercontent.com/stlink-org/stlink/master/config/udev/rules.d/49-stlinkv3.rules" -o /etc/udev/rules.d/49-stlinkv3.rules 2>/dev/null
    sudo udevadm control --reload-rules 2>/dev/null
    echo "✅ ST-Link udev 规则已配置"
fi

# 将用户加入串口组（不同发行版名称不同）
# Ubuntu/Debian = dialout, Arch = uucp
SERIAL_GROUP="dialout"
if [ "$OS" = "arch" ]; then
    SERIAL_GROUP="uucp"
fi
# 如果组不存在则创建
if ! getent group "$SERIAL_GROUP" > /dev/null 2>&1; then
    echo "📦 创建 $SERIAL_GROUP 组..."
    sudo groupadd "$SERIAL_GROUP"
fi
if ! groups | grep -q "$SERIAL_GROUP"; then
    echo "⚠️  添加用户到 $SERIAL_GROUP 组（串口权限）..."
    sudo usermod -aG "$SERIAL_GROUP" $USER
    echo "   请重新登录或运行 'newgrp $SERIAL_GROUP' 使权限生效"
fi

echo ""
echo "=============================="
echo "✅ STM32 Toolkit 安装完成!"
echo "=============================="
echo ""
echo "快速开始:"
echo "  source ~/.bashrc"
echo "  stm32make list                # 查看支持的芯片"
echo "  stm32make new led-blink F103C8  # 新建项目"
echo "  cd led-blink"
echo "  cmake -B build && cmake --build build  # 编译"
echo "  stm32make flash               # 自动烧录"
