#!/usr/bin/ruby

FILES = %w(relay) + %w(console fibre ringbuf list messageq util fibre_default libopencm3/console_cmd_gpio libopencm3/time_libopencm3 libopencm3/console_cdcacm).map { |f| "librfn/librfn/" + f }

ARCH = :stm32f1

case ARCH
when :stm32f1
  CFLAGS = "-mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd -DSTM32F1"
  LDSCRIPT = "stm32f103x8.ld"
  LIBS = "-lopencm3_stm32f1"
when :stm32f4
  CFLAGS = "-mthumb -mcpu=cortex-m4 mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4"
  LDSCRIPT = "stm32f405x6.ld"
  LIBS = "-lopencm3_stm32f4"
end

for f in FILES
  system("arm-none-eabi-gcc -Os -flto -g -std=c11 -Wextra -Wshadow -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes -fno-common -ffunction-sections -fdata-sections -DNDEBUG -DCONFIG_CONSOLE_FROM_ISR=1 -I librfn/include -MD -Wall -Wundef -DF_CPU=120000000 -DCONFIG_USB_MAX_POWER=500 #{CFLAGS} -o #{f}.o -c #{f}.c")
end

system("arm-none-eabi-gcc -flto --static -nostartfiles -T #{LDSCRIPT} -Wl,-Map=relay.map -Wl,--gc-sections #{CFLAGS} #{FILES.map { |f| f + ".o" }.join(" ")} #{LIBS} -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o relay.elf")

system("arm-none-eabi-objcopy -O binary relay.elf relay.bin")
system("arm-none-eabi-objdump -D -marm --disassembler-options=force-thumb --target binary relay.bin > relay.asm")
system("dfu-convert -b 0x08000000:relay.bin relay.dfu")
# st-flash write relay.bin 0x8000000
