# STM32 Toolkit — Embedded Development for Linux

[中文版](README_zh.md)

One-click STM32 project creation, compilation, and flashing on Linux. Supports Ubuntu / Debian / Fedora / Arch.

```bash
# 1. Clone
git clone https://github.com/HIKARIZG1102/stm32-toolkit.git
cd stm32-toolkit

# 2. One-click install (detects distro, installs deps, configures permissions)
bash setup.sh

# 3. Reload shell config
source ~/.bashrc

# 4. Create a project
stm32make new led-blink F103C8

# 5. Build
cd led-blink
cmake -B build && cmake --build build

# 6. Plug in ST-Link, flash
stm32make flash
```

## Features

- **`stm32make new <name> <chip>`** — Generate a complete project with vector table, linker script, and CMakeLists.txt
- **`stm32make flash`** — Auto-detect ST-Link or serial ISP, flash with one command
- **`stm32make doctor`** — Diagnose your dev environment (compiler, tools, permissions)
- **`stm32make update`** — Pull the latest version from GitHub
- **`stm32make list`** — List supported chip templates
- **SPL mode** (`--spl`) — Optional STM32 Standard Peripheral Library integration

Language auto-detection: displays Chinese or English based on `$LANG`.

## Supported Chips

| Chip | Command | Flash | RAM | Common Boards |
|:----|:--------|:------|:----|:--------------|
| STM32F103C8 | `stm32make new xxx F103C8` | 64K | 20K | Blue Pill, minimum dev board |
| STM32F103RCT6 | `stm32make new xxx F103RCT6` | 256K | 48K | Various dev boards |

## Command Reference

```bash
stm32make new led-blink F103C8           # New project (bare metal)
stm32make new led-blink F103C8 --spl     # New project (with SPL)
stm32make new robot F103RCT6             # For RCT6
stm32make list                           # List chip templates
stm32make flash                          # Auto-detect & flash
stm32make flash build/app.bin            # Flash specific file
stm32make flash --use-reset              # Flash with hardware reset (NRST required)
stm32make doctor                         # Diagnose environment
stm32make update                         # Update from GitHub
```

## Environment Diagnostics

```bash
stm32make doctor
```

Checks:
- arm-none-eabi-gcc / cmake / st-flash / stm32flash / python3
- ST-Link udev rules
- Serial port group membership
- Chip templates availability
- OS information

## Update

```bash
stm32make update
```

Runs `git pull` in the toolkit directory. Since `setup.sh` adds `~/stm32-toolkit/bin/` directly to your PATH, updates are available immediately after pulling.

## NRST Wiring Note

> `stm32make flash` uses `--connect-under-reset` by default, **not** `--reset`.
> This is because many DIY boards don't connect ST-Link's NRST pin to the MCU.
> Using `--reset` without NRST wiring will leave the chip halted after flashing.
>
> If your ST-Link has NRST connected, add `--use-reset`:
> ```bash
> stm32make flash --use-reset
> ```

## Project Structure

```
stm32-toolkit/
├── setup.sh                   ← One-click install
├── bin/stm32make              ← Project generator + flasher
├── packs/
│   ├── CMSIS/                 ← ARM Cortex core headers
│   └── STM32F1_SPL/           ← ST Standard Peripheral Library (SLA0044)
├── templates/                 ← Chip templates
│   ├── F103C8/                ← Blue Pill (bare metal)
│   ├── F103C8-spl/            ← Blue Pill (SPL)
│   ├── F103RCT6/              ← RCT6 (bare metal)
│   └── F103RCT6-spl/          ← RCT6 (SPL)
└── scripts/
```

Generated project structure:

```
project_name/
├── CMakeLists.txt
├── link.ld
├── inc/
├── src/main.c
└── build/
```

## Adding New Chips

```bash
cp -r templates/F103C8 templates/STM32F407VG
# Edit link.ld (Flash/RAM sizes)
# Edit CMakeLists.txt (-mcpu, -D macros)
# Edit src/main.c (SP address)
```

## Compatibility

| Distro | Package Manager | Status | Shell |
|:-------|:---------------|:-------|:------|
| Ubuntu 20.04+ | apt | ✅ Tested | bash / zsh |
| Debian 11+ | apt | ✅ Theoretical | bash / zsh |
| Fedora 35+ | dnf | ✅ Theoretical | bash / zsh |
| Arch Linux | pacman + AUR helper | ✅ Tested | bash / zsh / fish |
| Other | manual | ⚠️ Manual deps | — |

## Dependencies

| Tool | Purpose | Verify |
|:----|:--------|:-------|
| `arm-none-eabi-gcc` | ARM cross-compiler | `arm-none-eabi-gcc --version` |
| `cmake` | Build system | `cmake --version` |
| `stlink-tools` | ST-Link flashing | `st-info --probe` |
| `stm32flash` | Serial ISP flashing | `stm32flash --version` |
| `python3` | Scripting | `python3 --version` |

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
