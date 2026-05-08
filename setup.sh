#!/bin/bash
# setup.sh — STM32 Toolkit 一键环境安装
# 支持: Ubuntu, Debian, Fedora, Arch Linux
# 自动检测: shell (bash/fish/zsh), AUR 助手, 串口组名

set -e

echo "🔧 STM32 Toolkit 安装脚本"
echo "=========================="
echo ""

# ── 系统检测 ──
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VER=$VERSION_ID
else
    OS=$(uname -s)
fi

echo "📦 检测到系统: $NAME ($VERSION_ID)"
echo ""

# ── Shell 检测 ──
detect_shell_config() {
    local shell_name="$SHELL"
    case "$shell_name" in
        */bash) echo "$HOME/.bashrc" ;;
        */zsh)  echo "$HOME/.zshrc" ;;
        */fish) echo "$HOME/.config/fish/config.fish" ;;
        *)      echo "$HOME/.bashrc" ;;
    esac
}

SHELL_CONFIG=$(detect_shell_config)
echo "📝 检测到 Shell: $(basename "$SHELL") → 配置: $SHELL_CONFIG"
echo ""

# ── 包管理器检测 ──
ARCH_AUR_HELPER=""
if [ "$OS" = "arch" ]; then
    if command -v paru &>/dev/null; then
        ARCH_AUR_HELPER="paru"
    elif command -v yay &>/dev/null; then
        ARCH_AUR_HELPER="yay"
    fi
    if [ -n "$ARCH_AUR_HELPER" ]; then
        echo "🔍 检测到 AUR 助手: $ARCH_AUR_HELPER"
        echo ""
    fi
fi

# ── 串口组名 ──
SERIAL_GROUP="dialout"
[ "$OS" = "arch" ] && SERIAL_GROUP="uucp"

# ── 安装依赖 ──
install_pkg() {
    case "$OS" in
        ubuntu|debian)
            sudo apt update && sudo apt install -y gcc-arm-none-eabi cmake stlink-tools stm32flash python3 python3-serial
            ;;
        fedora)
            sudo dnf install -y arm-none-eabi-gcc arm-none-eabi-newlib cmake stlink stm32flash python3 pyserial
            ;;
        arch)
            local pacman_pkgs=(arm-none-eabi-gcc arm-none-eabi-newlib cmake stlink python python-pyserial)
            sudo pacman -S --noconfirm "${pacman_pkgs[@]}"
            if ! command -v stm32flash &>/dev/null; then
                if [ -n "$ARCH_AUR_HELPER" ]; then
                    echo "📥 通过 $ARCH_AUR_HELPER 安装 stm32flash (AUR)..."
                    "$ARCH_AUR_HELPER" -S --noconfirm stm32flash
                else
                    echo "⚠️  未检测到 AUR 助手 (yay/paru)"
                    echo "   请手动安装: yay -S stm32flash"
                fi
            fi
            ;;
        *)
            echo "⚠️  未识别的系统: $OS"
            echo "   请手动安装: arm-none-eabi-gcc, cmake, stlink-tools, stm32flash"
            echo "   然后重新运行此脚本"
            exit 1
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

# ── 下载 CMSIS 头文件 ──
CMSIS_FILES="core_cm3.h cmsis_gcc.h cmsis_compiler.h cmsis_version.h"
CMSIS_MISSING=false
for f in $CMSIS_FILES; do
    if [ ! -f "/usr/local/include/$f" ]; then
        CMSIS_MISSING=true
        break
    fi
done
if $CMSIS_MISSING; then
    echo "📥 下载 CMSIS 头文件..."
    sudo mkdir -p /usr/local/include
    for f in $CMSIS_FILES; do
        if [ ! -f "/usr/local/include/$f" ]; then
            sudo curl -sL \
                "https://raw.githubusercontent.com/ARM-software/CMSIS_5/master/CMSIS/Core/Include/$f" \
                -o "/usr/local/include/$f" || echo "⚠️  下载 $f 失败，跳过"
        fi
    done
    echo "✅ CMSIS 头文件已安装到 /usr/local/include/"
fi

# ── 添加 PATH + STM32_TOOLKIT 环境变量 ──
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TOOLKIT_BIN="$SCRIPT_DIR/bin"

# ── 确保 ~/bin 存在并创建 symlink（通用方案，不依赖 shell rc）──
mkdir -p "$HOME/bin"
if [ ! -L "$HOME/bin/stm32make" ]; then
    ln -sf "$TOOLKIT_BIN/stm32make" "$HOME/bin/stm32make" 2>/dev/null && \
        echo "✅ 已创建 ~/bin/stm32make -> $TOOLKIT_BIN/stm32make (软链接)"
fi

# ── 写入各 shell 配置文件 ──
case "$(basename "$SHELL")" in
    fish)
        if ! grep -q "stm32-toolkit" "$SHELL_CONFIG" 2>/dev/null; then
            {
                echo ""
                echo "# STM32 Toolkit"
                echo "fish_add_path $TOOLKIT_BIN"
                echo "set -gx STM32_TOOLKIT $SCRIPT_DIR"
            } >> "$SHELL_CONFIG"
            echo "✅ 已添加 PATH 到 $SHELL_CONFIG"
        fi
        ;;
    zsh)
        # ~/.zshrc: interactive zsh
        if ! grep -q "stm32-toolkit" ~/.zshrc 2>/dev/null; then
            {
                echo ""
                echo "# STM32 Toolkit"
                echo "export PATH=\"\$PATH:$TOOLKIT_BIN\""
                echo "export STM32_TOOLKIT=\"$SCRIPT_DIR\""
            } >> ~/.zshrc
            echo "✅ 已添加 PATH 到 ~/.zshrc"
        fi
        # ~/.zshenv: all zsh instances (VSCode tasks, scripts)
        if ! grep -q "stm32-toolkit" ~/.zshenv 2>/dev/null; then
            {
                echo ""
                echo "# STM32 Toolkit"
                echo "export PATH=\"\$PATH:$TOOLKIT_BIN\""
                echo "export STM32_TOOLKIT=\"$SCRIPT_DIR\""
            } >> ~/.zshenv
            echo "✅ 已添加 PATH 到 ~/.zshenv（zsh 全模式）"
        fi
        ;;
    *)
        # bash / sh / dash: ~/.bashrc for interactive, ~/.profile for login
        if ! grep -q "stm32-toolkit" ~/.bashrc 2>/dev/null; then
            {
                echo ""
                echo "# STM32 Toolkit"
                echo "export PATH=\"\$PATH:$TOOLKIT_BIN\""
                echo "export STM32_TOOLKIT=\"$SCRIPT_DIR\""
            } >> ~/.bashrc
            echo "✅ 已添加 PATH 到 ~/.bashrc"
        fi
        if ! grep -q "stm32-toolkit" ~/.profile 2>/dev/null; then
            {
                echo ""
                echo "# STM32 Toolkit"
                echo "export PATH=\"\$PATH:$TOOLKIT_BIN\""
                echo "export STM32_TOOLKIT=\"$SCRIPT_DIR\""
            } >> ~/.profile
            echo "✅ 已添加 PATH 到 ~/.profile（登录 shell / VSCode）"
        fi
        ;;
esac

echo ""
echo "💡 关于 VSCode 集成终端找不到 stm32make:"
echo "   ~/bin/stm32make 软链接已创建，如果 ~/bin 已在 PATH 中则无需额外配置。"
echo "   若仍不生效，VSCode 设置中启用:"
echo "   \"terminal.integrated.shellArgs.linux\": [\"-l\"]"
echo "   或手动执行: source ~/.profile"

# ── 添加 udev 规则（ST-Link 权限）──
if [ ! -f /etc/udev/rules.d/49-stlinkv3.rules ]; then
    echo "📥 配置 ST-Link udev 规则..."
    sudo curl -sL \
        "https://raw.githubusercontent.com/stlink-org/stlink/master/config/udev/rules.d/49-stlinkv3.rules" \
        -o /etc/udev/rules.d/49-stlinkv3.rules 2>/dev/null || \
        echo "⚠️  下载 udev 规则失败"
    sudo udevadm control --reload-rules 2>/dev/null || true
    echo "✅ ST-Link udev 规则已配置"
fi

# ── 将用户加入串口组 ──
if ! getent group "$SERIAL_GROUP" > /dev/null 2>&1; then
    echo "📦 创建 $SERIAL_GROUP 组..."
    sudo groupadd "$SERIAL_GROUP"
fi
if ! groups | grep -q "\b$SERIAL_GROUP\b"; then
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
echo ""
case "$(basename "$SHELL")" in
    fish)
        echo "  source ~/.config/fish/config.fish"
        ;;
    *)
        echo "  source ~/.bashrc"
        ;;
esac
echo "  stm32make list                  # 查看支持的芯片"
echo "  stm32make new led-blink F103C8  # 新建项目"
echo "  cd led-blink"
echo "  cmake -B build && cmake --build build  # 编译"
echo "  stm32make flash                 # 自动烧录"
echo ""
echo "注意事项:"
echo "  - 串口权限组: $SERIAL_GROUP (需重新登录生效)"
echo "  - 如果使用 Fish shell, 不要用 'source ~/.bashrc'"
echo "  - STM32_TOOLKIT 环境变量已设置（-spl 模板需要）"
