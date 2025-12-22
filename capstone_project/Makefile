DEVICE     = atmega328p
CLOCK      = 16000000
PROGRAMMER = -c arduino -b 115200 -P COM6
OBJECTS    = $(SRC:.c=.o)
FUSES      = -U hfuse:w:0xde:m -U lfuse:w:0xff:m -U efuse:w:0x05:m

# File names
TARGET = main
SRC = \
    $(wildcard *.c) \
    $(wildcard drivers/*.c) \
    $(wildcard drivers/application/*.c) \
    $(wildcard drivers/hardware/*.c)
LDFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm

# Include paths
INCLUDES = \
    -Idrivers \
    -Idrivers/application \
    -Idrivers/hardware

# Tune the lines below only if you know what you are doing:
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -std=c99 -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) $(INCLUDES)

$(TARGET).elf: $(OBJECTS)
	$(COMPILE) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(COMPILE) -c $< -o $@

# symbolic targets:
all:	main.hex

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
