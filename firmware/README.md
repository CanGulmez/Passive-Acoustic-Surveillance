# Embedded Firmware of Acoustic Surveillance System

Embedded firmware was written for STM32H75x series microcontroller. There is no
externel dependencies for the firmware building, flashing and running. The build
system is Makefile-based. So that make sure that `make` command is avaliable.

Overall, there should be installed the STM32CubeH7 library installed with:

```bash
$ git clone --recurse-submodules https://github.com/STMicroelectronics/STM32CubeH7.git
```

After installed the library, extracted the CMSIS, FreeRTOS and HAL modules and then
put these under `./driver`.

The firmware building is done over two `make` commands. First step is to make the
static library including the CMSIS, FreeRTOS and HAL components. It's done often for
one time. After that, the main program is being compiled as linking with the library.
It must be done after the any changes were being made.

```bash
$ make lib					# build the static library
$ make bin					# build the firmware program
```

Lastly, the firmware is flashed into the microcontroller in a way. Makefile currently
don't dictate any way. But mostly it is done over `openocd` tool:

```bash
$ openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.elf verify reset exit"
```

Above command is a short-hand way to open the `openocd` session and flash it quickly.

If the debugger is wanted, these `openocd` and `arm-none-eabi-gdb` must be used
separately.

