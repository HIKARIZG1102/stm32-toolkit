# STM32 Toolkit — Embedded Development for Linux

[中文版](README_zh.md)

One-click STM32 project creation, compilation, and flashing on Linux. Supports bare-metal (no libraries), SPL (Standard Peripheral Library), and HAL (Hardware Abstraction Layer) modes.

```bash
# 1. Clone
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 2. One-click install (detects distro, installs deps, configures permissions)
bash setup.sh

# 3. Reload shell config
source ~/.profile

# 4. Create a project
stm32make new led-blink F103C8

# 5. Build
cd led-blink && stm32make build

# 6. Plug in ST-Link, flash
stm32make flash
```

## Quick Start

```bash
# ── Bare-metal (no library, minimal) ──
stm32make new myproj F103RCT6

# ── SPL mode (Standard Peripheral Library) ──
stm32make new myproj F103RCT6 --spl

# ── HAL mode (Hardware Abstraction Layer) ──
stm32make new myproj F103RCT6 --hal

# ── Build & Flash ──
cd myproj
stm32make build          # Build
stm32make flash          # Auto-detect ST-Link or serial ISP, flash
```

## Features

| Command | Description |
|:--------|:------------|
| `stm32make new <name> <chip>` | Generate project (bare metal, default) |
| `stm32make new <name> <chip> --spl` | Generate project with SPL library |
| `stm32make new <name> <chip> --hal` | Generate project with HAL library (STM32CubeF1) |
| `stm32make build` | One-click build from project directory |
| `stm32make flash` | Auto-detect ST-Link or serial ISP, flash |
| `stm32make flash --use-reset` | Flash with hardware reset (requires NRST wire) |
| `stm32make flash build/app.bin` | Flash a specific .bin file |
| `stm32make list` | List supported chip templates |
| `stm32make doctor` | Diagnose development environment |
| `stm32make update` | Pull latest version from GitHub |

Language auto-detection: displays Chinese or English based on `$LANG`.

## Supported Chips

| Chip | Command | Flash | RAM | Common Boards |
|:-----|:--------|:------|:----|:--------------|
| STM32F103C8 | `stm32make new xxx F103C8` | 64K | 20K | Blue Pill, minimum system board |
| STM32F103RCT6 | `stm32make new xxx F103RCT6` | 256K | 48K | MiniSTM32, various dev boards |

New chips can be added by creating a new template directory (see "Adding New Chips").

## Generated Project Structure

**Bare-metal / SPL mode:**

```
myproj/
├── CMakeLists.txt          ← Build config (mcpu, flags, sources)
├── link.ld                 ← Linker script (Flash/RAM, .data/.bss)
├── inc/
│   ├── board.h             ← **Your board's pin mappings (edit this!)**
│   └── stm32f10x_conf.h    ← SPL module selection (all enabled by default)
├── src/
│   └── main.c              ← Application entry (uses board.h macros)
├── Drivers/
│   ├── CMSIS/              ← ARM Cortex-M core headers + system_stm32f10x.c
│   └── SPL/                ← Standard Peripheral Library (when --spl)
└── build/
    └── myproj.bin          ← Compiled binary (ready to flash)
```

**HAL mode:**

```
myproj/
├── CMakeLists.txt
├── link.ld
├── inc/
│   └── stm32f1xx_hal_conf.h
├── src/
│   ├── main.c              ← Application (full HAL demo: LED + button)
│   ├── startup.c           ← Full vector table + .data/.bss init
│   └── system_stm32f10x.c  ← Clock configuration (SystemInit → 72MHz PLL)
├── Drivers/
│   ├── CMSIS/              ← CMSIS_F1 headers
│   └── STM32F1_HAL_Driver/ ← STM32CubeF1 HAL library
└── build/
```

### Template Evolution

The most recent template improvements ensure:

- **No hardcoded SP** — Stack pointer is auto-computed from linker script (`_estack = ORIGIN(RAM) + LENGTH(RAM)`), works across different chips without manual adjustment
- **SystemInit() is called** — Generated projects run at 72MHz (PLL from 8MHz HSE) instead of 8MHz HSI
- **.data/.bss sections** — Global/static variables are properly initialized (zeroed / copied from flash)
- **CMSIS integrated** — `system_stm32f10x.c` is included and compiled automatically
- **Full vector table** — In HAL mode, startup.c provides 82 interrupt vectors with weak `Default_Handler` fallback — just define the IRQHandler you need

## Environment Diagnostics

```bash
stm32make doctor
```

Checks:
- arm-none-eabi-gcc / cmake / st-flash / stm32flash / python3
- ST-Link udev rules
- Serial port group membership (dialout / uucp)
- Chip template availability
- OS information

## Update

```bash
stm32make update
```

Runs `git pull` in the toolkit directory. A `~/bin/stm32make` symlink points to the repo version, so updates are immediately available.

## VSCode Integration

If VSCode's integrated terminal cannot find `stm32make`:

1. **Symlink method (recommended):** The setup script creates `~/bin/stm32make → stm32-toolkit/bin/stm32make`. If `~/bin` is already in your PATH (default on Ubuntu), restart VSCode and it should work.

2. **VSCode setting:** Enable login shell mode:
   ```
   "terminal.integrated.shellArgs.linux": ["-l"]
   ```

3. **Manual:** Run `source ~/.profile` in the terminal, or reopen VSCode.

## NRST Wiring Note

> `stm32make flash` uses `--connect-under-reset` by default, **not** `--reset`.
> This is because many DIY boards don't connect ST-Link's NRST pin to the MCU.
> Using `--reset` without NRST wiring will leave the chip halted after flashing.
>
> If your ST-Link has NRST connected, add `--use-reset`:
> ```bash
> stm32make flash --use-reset
> ```

## Architecture: Chip vs Board

The toolkit separates **chip-level** (CPU core, Flash/RAM sizes, startup files) from **board-level** (LED pins, USART pins, button mappings).

```
templates/     ← Chip-level only — contributed by maintainers
boards/        ← Board-level pin definitions — contributed by community
examples/      ← Full working projects — contributed by anyone
```

When you create a project, the template generates `inc/board.h` with sensible defaults. If your board uses different pins, just edit that one file instead of modifying the application logic.

Standard board definitions are in `boards/` for quick reference:

```bash
# Use Blue Pill pinout as reference
cat boards/bluepill.h

# Use MiniSTM32 RCT6 pinout
cat boards/mini_stm32_rct6.h
```

### How to port a project to a different board

1. Edit `inc/board.h` — change LED, USART, and button macros
2. Rebuild — `stm32make build`
3. That's it — no code changes needed in `main.c`

## Example Projects

The repository includes ready-to-build example projects in the `examples/` directory:

| Project | Mode | Description |
|:--------|:-----|:------------|
| `TIM_General` | SPL | TIM2 general timer, 1s LED blink (ISR counter + main loop polling) |
| `TIM_Advanced` | SPL | TIM1 advanced timer, dual independent timing (500ms + 1000ms on one LED) |
| `ADC_Temp` | SPL | ADC1 internal temperature sensor (channel 16), DMA circular transfer |
| `ADC_Light` | SPL | ADC1 external light sensor (channel 0, PA0), conversion complete interrupt |

Each project includes USART1 printf debug output and can be built with:

```bash
cd examples/TIM_General
stm32make build && stm32make flash
```

## Serial Flashing (ISP Mode)

When no ST-Link is detected, `stm32make flash` automatically falls back to serial ISP:

1. Drives CH340 DTR/RTS to enter the built-in bootloader
2. Flashes via `stm32flash` over UART
3. Requires: BOOT0=HIGH (bootloader), serial connection to USART1 (PA9/PA10)

## Toolkit Directory Structure

```
stm32-toolkit/
├── setup.sh                   ← One-click install
├── bin/stm32make              ← Project generator + flasher
├── packs/
│   ├── CMSIS/                 ← ARM Cortex core headers + system_stm32f10x.c
│   ├── CMSIS_F1/              ← CMSIS for HAL (STM32CubeF1)
│   ├── STM32F1_SPL/           ← ST Standard Peripheral Library
│   └── STM32F1_HAL_Driver/    ← STM32CubeF1 HAL drivers
├── templates/
│   ├── F103C8/                ← Blue Pill (bare metal)
│   ├── F103C8-spl/            ← Blue Pill (SPL)
│   ├── F103RCT6/              ← RCT6 (bare metal)
│   ├── F103RCT6-spl/          ← RCT6 (SPL)
│   ├── F103C8-hal/            ← Blue Pill (HAL)
│   └── F103RCT6-hal/          ← RCT6 (HAL)
└── scripts/
```

## Adding New Chips

```bash
cp -r templates/F103RCT6-spl templates/STM32F407VG
# Edit link.ld (Flash/RAM sizes, memory map)
# Edit CMakeLists.txt (-mcpu, -D macros, chip-specific compilation flags)
```

The template system is modular. Create a new directory in `templates/`:
- `CHIPNAME/` → bare metal (no libraries)
- `CHIPNAME-spl/` → SPL library mode
- `CHIPNAME-hal/` → HAL mode

Each mode is independent. You only need to create the modes your chip supports.

## Compatibility

| Distro | Package Manager | Status | Shell Support |
|:-------|:---------------|:-------|:--------------|
| Ubuntu 20.04+ | apt | ✅ Tested | bash / zsh |
| Debian 11+ | apt | ✅ Theoretical | bash / zsh |
| Fedora 35+ | dnf | ✅ Theoretical | bash / zsh |
| Arch Linux | pacman + yay/paru | ✅ Tested | bash / zsh / fish |
| Other | manual install | ⚠️ Manual deps | — |

## Dependencies

| Tool | Purpose | Verify |
|:-----|:--------|:-------|
| `arm-none-eabi-gcc` | ARM cross-compiler | `arm-none-eabi-gcc --version` |
| `cmake` | Build system | `cmake --version` |
| `stlink-tools` | ST-Link flashing | `st-info --probe` |
| `stm32flash` | Serial ISP flashing | `stm32flash --version` |
| `python3` + `pyserial` | Scripting + serial control | `python3 -c "import serial"` |

## Known Issues

### 1. VTOR alignment requirement (gcc / Linux only)

The ARM Cortex-M3 Vector Table Offset Register (VTOR) requires the vector table base address to be **512-byte aligned**. This is not an issue on Windows (CubeIDE/CubeMX use the on-chip flash vector table at `0x08000000`, which is naturally aligned).

When using **HAL with RAM vector table** on gcc (e.g. to work around the SysTick interrupt issue described below), you must declare the RAM vector table with 512-byte alignment:

```c
static uint32_t ram_vector[256] __attribute__((aligned(512)));
```

Without this attribute, the linker may place the array at a non-aligned address (e.g. `0x2000002C`), causing VTOR writes to be silently ignored by the hardware, which results in "SysTick interrupt breaks GPIO" symptoms — the LED stays on but never blinks, and no visible error occurs.

### 2. SystemCoreClock must be set before HAL_Init()

When using the internal HSI oscillator (8 MHz, default after reset), `SystemCoreClock` in `.data` segment has initial value `72000000` (configured for 72 MHz operation). If `HAL_Init()` is called without first setting `SystemCoreClock = 8000000`, the SysTick reload value is calculated as `72000000 / 1000 = 72000`. With the actual 8 MHz HCLK, this results in a 9 ms tick period (not 1 ms), making `HAL_Delay(300)` wait 2.7 seconds instead of 300 ms.

**Fix:** Always set `SystemCoreClock` before `HAL_Init()`:

```c
SystemCoreClock = 8000000;
HAL_Init();
```

### 3. SPL template includes CMSIS source files

When using `--spl` mode, the template automatically compiles `Drivers/CMSIS/*.c` (which includes `system_stm32f10x.c` for the `SystemInit()` function). If you see linker errors about `SystemInit` being undefined, ensure the build output includes CMSIS source compilation. This is now handled automatically in the template CMakeLists.txt.

### 4. Global variables require .bss initialization

The updated linker script includes `.data` and `.bss` sections, and the startup code (HAL mode's `startup.c` / SPL mode's `Reset_Handler`) initializes them. If you are porting old projects that hardcoded stack pointer values, update your linker script and main.c to use `_estack` from the linker script instead.
