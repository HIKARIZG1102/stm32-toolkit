     1|#!/bin/bash
     2|# setup.sh — STM32 Toolkit 一键环境安装
     3|# 支持: Ubuntu, Debian, Fedora, Arch Linux
     4|# 自动检测: shell (bash/fish/zsh), AUR 助手, 串口组名
     5|
     6|set -e
     7|
     8|echo "🔧 STM32 Toolkit 安装脚本"
     9|echo "=========================="
    10|echo ""
    11|
    12|# ── 系统检测 ──
    13|if [ -f /etc/os-release ]; then
    14|    . /etc/os-release
    15|    OS=$ID
    16|    VER=$VERSION_ID
    17|else
    18|    OS=$(uname -s)
    19|fi
    20|
    21|echo "📦 检测到系统: $NAME ($VERSION_ID)"
    22|echo ""
    23|
    24|# ── Shell 检测 ──
    25|detect_shell_config() {
    26|    local shell_name="$SHELL"
    27|    case "$shell_name" in
    28|        */bash)
    29|            echo "$HOME/.bashrc"
    30|            ;;
    31|        */zsh)
    32|            echo "$HOME/.zshrc"
    33|            ;;
    34|        */fish)
    35|            echo "$HOME/.config/fish/config.fish"
    36|            ;;
    37|        *)
    38|            echo "$HOME/.bashrc"
    39|            ;;
    40|    esac
    41|}
    42|
    43|SHELL_CONFIG=$(detect_shell_config)
    44|echo "📝 检测到 Shell: $(basename "$SHELL") → 配置: $SHELL_CONFIG"
    45|echo ""
    46|
    47|# ── 包管理器检测 ──
    48|ARCH_AUR_HELPER=""
    49|if [ "$OS" = "arch" ]; then
    50|    if command -v paru &>/dev/null; then
    51|        ARCH_AUR_HELPER="paru"
    52|    elif command -v yay &>/dev/null; then
    53|        ARCH_AUR_HELPER="yay"
    54|    fi
    55|    if [ -n "$ARCH_AUR_HELPER" ]; then
    56|        echo "🔍 检测到 AUR 助手: $ARCH_AUR_HELPER"
    57|        echo ""
    58|    fi
    59|fi
    60|
    61|# ── 串口组名 ──
    62|SERIAL_GROUP="dialout"
    63|[ "$OS" = "arch" ] && SERIAL_GROUP="uucp"
    64|
    65|# ── 安装依赖 ──
    66|install_pkg() {
    67|    case "$OS" in
    68|        ubuntu|debian)
    69|            sudo apt update && sudo apt install -y gcc-arm-none-eabi cmake stlink-tools stm32flash python3 python3-serial
    70|            ;;
    71|        fedora)
    72|            sudo dnf install -y arm-none-eabi-gcc arm-none-eabi-newlib cmake stlink stm32flash python3 pyserial
    73|            ;;
    74|        arch)
    75|            # pacman 官方源包
    76|            local pacman_pkgs=(
    77|                arm-none-eabi-gcc
    78|                arm-none-eabi-newlib
    79|                cmake
    80|                stlink
    81|                python
    82|                python-pyserial
    83|            )
    84|            sudo pacman -S --noconfirm "${pacman_pkgs[@]}"
    85|
    86|            # AUR 包: stm32flash
    87|            if ! command -v stm32flash &>/dev/null; then
    88|                if [ -n "$ARCH_AUR_HELPER" ]; then
    89|                    echo "📥 通过 $ARCH_AUR_HELPER 安装 stm32flash (AUR)..."
    90|                    "$ARCH_AUR_HELPER" -S --noconfirm stm32flash
    91|                else
    92|                    echo "⚠️  未检测到 AUR 助手 (yay/paru)"
    93|                    echo "   请手动安装: yay -S stm32flash"
    94|                    echo "   或从 https://aur.archlinux.org/packages/stm32flash 下载"
    95|                fi
    96|            else
    97|                echo "✅ stm32flash 已安装"
    98|            fi
    99|            ;;
   100|        *)
   101|            echo "⚠️  未识别的系统: $OS"
   102|            echo "   请手动安装: arm-none-eabi-gcc, cmake, stlink-tools, stm32flash"
   103|            echo "   然后重新运行此脚本"
   104|            exit 1
   105|            ;;
   106|    esac
   107|}
   108|
   109|echo "🚀 即将安装以下工具:"
   110|echo "   - ARM GCC 交叉编译器"
   111|echo "   - CMake 构建系统"
   112|echo "   - ST-Link 烧录工具"
   113|echo "   - stm32flash (串口ISP)"
   114|echo "   - Python3 + pyserial"
   115|echo ""
   116|echo -n "继续安装? [Y/n]: "
   117|read -r CONFIRM
   118|if [ "$CONFIRM" = "n" ] || [ "$CONFIRM" = "N" ]; then
   119|    echo "已取消"
   120|    exit 1
   121|fi
   122|
   123|install_pkg
   124|
   125|# ── 下载 CMSIS 头文件 ──
   126|CMSIS_FILES="core_cm3.h cmsis_gcc.h cmsis_compiler.h cmsis_version.h"
   127|CMSIS_MISSING=false
   128|for f in $CMSIS_FILES; do
   129|    if [ ! -f "/usr/local/include/$f" ]; then
   130|        CMSIS_MISSING=true
   131|        break
   132|    fi
   133|done
   134|
   135|if $CMSIS_MISSING; then
   136|    echo "📥 下载 CMSIS 头文件..."
   137|    sudo mkdir -p /usr/local/include
   138|    for f in $CMSIS_FILES; do
   139|        if [ ! -f "/usr/local/include/$f" ]; then
   140|            sudo curl -sL \
   141|                "https://raw.githubusercontent.com/ARM-software/CMSIS_5/master/CMSIS/Core/Include/$f" \
   142|                -o "/usr/local/include/$f" || echo "⚠️  下载 $f 失败，跳过"
   143|        fi
   144|    done
   145|    echo "✅ CMSIS 头文件已安装到 /usr/local/include/"
   146|fi
   147|
   148|# ── 添加 PATH ──
   149|SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
   150|TOOLKIT_BIN="$SCRIPT_DIR/bin"
   151|PATH_LINE="export PATH=\"\$PATH:$TOOLKIT_BIN\""
   152|
   153|case "$(basename "$SHELL")" in
   154|    fish)
   155|        # Fish 用 fish_add_path 或 set -gx PATH
   156|        if grep -q "stm32-toolkit" "$SHELL_CONFIG" 2>/dev/null; then
   157|            echo "✅ PATH 已在 $SHELL_CONFIG 中"
   158|        else
   159|            echo "" >> "$SHELL_CONFIG"
   160|            echo "# STM32 Toolkit" >> "$SHELL_CONFIG"
   161|            echo "fish_add_path $TOOLKIT_BIN" >> "$SHELL_CONFIG"
   162|            echo "✅ 已添加 PATH 到 $SHELL_CONFIG (重新登录或 'source $SHELL_CONFIG' 生效)"
   163|        fi
   164|        ;;
   165|    *)
   166|        # bash / zsh
   167|        if ! grep -q "stm32-toolkit" ~/.bashrc 2>/dev/null; then
   168|            echo "" >> ~/.bashrc
   169|            echo "# STM32 Toolkit" >> ~/.bashrc
   170|            echo "$PATH_LINE" >> ~/.bashrc
   171|            echo "✅ 已添加 PATH 到 ~/.bashrc (重新登录或 source ~/.bashrc 生效)"
   172|        fi
   173|        if [ "$(basename "$SHELL")" = "zsh" ] && [ -f ~/.zshrc ]; then
   174|            if ! grep -q "stm32-toolkit" ~/.zshrc 2>/dev/null; then
   175|                echo "" >> ~/.zshrc
   176|                echo "# STM32 Toolkit" >> ~/.zshrc
   177|                echo "$PATH_LINE" >> ~/.zshrc
   178|                echo "✅ 已添加 PATH 到 ~/.zshrc"
   179|            fi
   180|        fi
   181|        ;;
   182|esac
   183|
   184|# ── 添加 udev 规则（ST-Link 权限）──
   185|if [ ! -f /etc/udev/rules.d/49-stlinkv3.rules ]; then
   186|    echo "📥 配置 ST-Link udev 规则..."
   187|    sudo curl -sL \
   188|        "https://raw.githubusercontent.com/stlink-org/stlink/master/config/udev/rules.d/49-stlinkv3.rules" \
   189|        -o /etc/udev/rules.d/49-stlinkv3.rules 2>/dev/null || \
   190|        echo "⚠️  下载 udev 规则失败（ST-Link 可能需要手动配置权限）"
   191|    sudo udevadm control --reload-rules 2>/dev/null || true
   192|    echo "✅ ST-Link udev 规则已配置"
   193|fi
   194|
   195|# ── 将用户加入串口组 ──
   196|if ! getent group "$SERIAL_GROUP" > /dev/null 2>&1; then
   197|    echo "📦 创建 $SERIAL_GROUP 组..."
   198|    sudo groupadd "$SERIAL_GROUP"
   199|fi
   200|if ! groups | grep -q "\b$SERIAL_GROUP\b"; then
   201|    echo "⚠️  添加用户到 $SERIAL_GROUP 组（串口权限）..."
   202|    sudo usermod -aG "$SERIAL_GROUP" $USER
   203|    echo "   请重新登录或运行 'newgrp $SERIAL_GROUP' 使权限生效"
   204|fi
   205|
   206|echo ""
   207|echo "=============================="
   208|echo "✅ STM32 Toolkit 安装完成!"
   209|echo "=============================="
   210|echo ""
   211|echo "快速开始:"
   212|echo ""
   213|case "$(basename "$SHELL")" in
   214|    fish)
   215|        echo "  source \"\$XDG_CONFIG_HOME/fish/config.fish\""
   216|        echo "  # 或直接打开新终端"
   217|        ;;
   218|    *)
   219|        echo "  source ~/.bashrc"
   220|        ;;
   221|esac
   222|echo "  stm32make list                  # 查看支持的芯片"
   223|echo "  stm32make new led-blink F103C8  # 新建项目"
   224|echo "  cd led-blink"
   225|echo "  cmake -B build && cmake --build build  # 编译"
   226|echo "  stm32make flash                 # 自动烧录"
   227|echo ""
   228|echo "注意事项:"
   229|echo "  - 串口权限组: $SERIAL_GROUP (需重新登录生效)"
   230|echo "  - 如果使用 Fish shell, 不要用 'source ~/.bashrc'"
   231|echo "  - 用 'source ~/.config/fish/config.fish' 或 'exec fish' 重载"
   232|