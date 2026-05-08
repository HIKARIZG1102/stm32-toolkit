# Contributing to STM32 Toolkit

Thank you for helping grow the toolkit! This guide explains how to contribute correctly and maintain quality across chip templates, board definitions, and examples.

## Three Layers: Know Where Your Contribution Belongs

```
templates/     ← Chip-level only (CPU core, Flash/RAM sizes, startup files)
boards/        ← Board-level (pin mappings specific to a PCB)
examples/      ← Full working projects (can be board-specific, must document hardware)
```

| You want to add... | Put it in... | Must NOT contain |
|-------------------|-------------|-----------------|
| A new STM32 chip variant | `templates/CHIPNAME[-spl][-hal]/` | Board-specific pin numbers, custom PCB wiring |
| Your custom PCB's pinout | `boards/your_board.h` | Chip-specific startup code or linker scripts |
| A demo project | `examples/your_project/` | Hardcoded absolute paths, unlicensed third-party code |

## Contributing Chip Templates

A chip template includes:

| File | Required? | Must be correct for... |
|------|-----------|----------------------|
| `link.ld` | ✅ | Exact Flash/RAM sizes of this chip variant |
| `CMakeLists.txt` | ✅ | CPU core (`-mcpu`), density macro (`-DSTM32F10X_HD` etc.) |
| `inc/board.h` | ✅ | Most common pinout for this chip (reference only, user edits) |
| `inc/stm32f10x_conf.h` | ✅ (SPL) | All peripherals enabled (user disables as needed) |
| `src/main.c` | ✅ | Uses `board.h` macros, no hardcoded pins |
| `src/startup.c` | ✅ (HAL) | Correct vector table size for this density |
| `src/system_stm32f10x.c` | ✅ (HAL) | Correct HSE value and PLL settings |

### Checklist Before Submitting a Chip Template

- [ ] Flash/RAM sizes in `link.ld` match the chip datasheet exactly
- [ ] `-mcpu` flag matches the CPU core (cortex-m0/m3/m4/m7)
- [ ] Startup vector table has the correct number of entries for this density
- [ ] `main.c` uses macros from `board.h`, does NOT hardcode GPIO ports/pins
- [ ] The template builds without errors: `stm32make new test CHIPNAME && cd test && stm32make build`
- [ ] The template flashes and runs: LED blinks, no crash
- [ ] Default `board.h` uses the most common pinout for this chip

### Density Classification (STM32F1)

| Density | Chips | Flash | SRAM | Vector Table |
|---------|-------|-------|------|-------------|
| LD (Low) | F101/2/3x4/6 | 16-32K | 4-10K | 45 entries |
| MD (Medium) | F103x8/B | 64-128K | 20K | 51 entries |
| HD (High) | F103xC/D/E | 256-512K | 48-64K | 82 entries |
| XL (XL) | F103xF/G | 768-1024K | 96K | 91 entries |
| CL (Connectivity) | F105/7 | 256K | 64K | 92 entries |

Use your chip's density to determine the correct `-DSTM32F10X_XX` macro and vector table size.

## Contributing Board Definitions

Board files go in `boards/` and only contain pin mappings:

```c
// boards/my_custom_board.h — One STM32F103C8 board with custom LED
#ifndef BOARD_MY_CUSTOM_BOARD_H
#define BOARD_MY_CUSTOM_BOARD_H

#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        GPIO_Pin_0     // My board has LED on PB0
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOB

#define USARTx              USART1
#define USART_TX_PIN        GPIO_Pin_9
// ... etc

#define HSE_FREQ_HZ         8000000
#endif
```

### Checklist Before Submitting a Board

- [ ] File is in `boards/` with a descriptive name
- [ ] Uses include guard: `#ifndef BOARD_YOUR_BOARD_H`
- [ ] Only contains macros (no code, no functions)
- [ ] Comments document the physical PCB (link to schematic if possible)
- [ ] Does NOT duplicate chip-specific settings (no startup code, no link.ld)

## Contributing Examples

Examples are full projects that demonstrate a specific peripheral or technique.

### Checklist Before Submitting an Example

- [ ] Project builds with `stm32make build` (no external dependencies)
- [ ] Project flashes with `stm32make flash`
- [ ] `README.md` documents: required hardware, wiring, expected behavior
- [ ] Source code is clean, commented, and follows a consistent style
- [ ] Does NOT contain absolute paths (no `/home/user/` references)
- [ ] Does NOT contain build artifacts (no `build/` directory, no `.o` files)

## Code Style Guidelines

- **C standard**: C11 (`-std=gnu11`)
- **Indentation**: 4 spaces, no tabs
- **Naming**: `snake_case` for functions and variables, `UPPER_CASE` for macros
- **Braces**: Allman style (opening brace on its own line) for functions, K&R for control flow
- **Comments**: English only (Chinese comments OK in Chinese README files)

## PR Review Process

1. Open a PR with a clear description of what you're adding and why
2. The CI will automatically build the template/example (if CI is configured)
3. At least one maintainer must review:
   - Correctness of chip parameters (Flash/RAM, CPU core, density)
   - That board-specific code doesn't leak into chip templates
   - That examples document their hardware requirements
4. Once approved, the maintainer merges

## Questions?

Open an issue or discussion on GitHub. We're happy to help you contribute!
