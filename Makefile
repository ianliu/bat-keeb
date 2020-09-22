-include env.make

BUILDDIR ?= build
PORT     ?= /dev/ttyACM0
BUILD    ?= debug

CXX = avr-g++
CC = avr-gcc
MCU = atmega32u4

_CPPFLAGS = -DF_CPU=16000000L -DARDUINO=10813 -DARDUINO_AVR_LEONARDO -DARDUINO_ARCH_AVR \
	    -DUSB_VID=0x2341 -DUSB_PID=0x8036 "-DUSB_MANUFACTURER=\"Unknown\"" \
	    "-DUSB_PRODUCT=\"Arduino Leonardo\"" -I. $(CPPFLAGS)

_CXXFLAGS = -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections \
	    -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto \
	    -mmcu=$(MCU) -I. $(CXXFLAGS)

_CFLAGS = -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -fno-fat-lto-objects \
	  -MMD -flto -mmcu=$(MCU) $(CFLAGS)

LDFLAGS =
LIBS = -lm

CORE = $(BUILDDIR)/abi.o \
       $(BUILDDIR)/CDC.o \
       $(BUILDDIR)/HardwareSerial0.o \
       $(BUILDDIR)/HardwareSerial1.o \
       $(BUILDDIR)/HardwareSerial2.o \
       $(BUILDDIR)/HardwareSerial3.o \
       $(BUILDDIR)/HardwareSerial.o \
       $(BUILDDIR)/IPAddress.o \
       $(BUILDDIR)/hooks.o \
       $(BUILDDIR)/main.o \
       $(BUILDDIR)/new.o \
       $(BUILDDIR)/PluggableUSB.o \
       $(BUILDDIR)/Print.o \
       $(BUILDDIR)/Stream.o \
       $(BUILDDIR)/Tone.o \
       $(BUILDDIR)/USBCore.o \
       $(BUILDDIR)/WInterrupts.o \
       $(BUILDDIR)/wiring_analog.o \
       $(BUILDDIR)/wiring.o \
       $(BUILDDIR)/wiring_digital.o \
       $(BUILDDIR)/wiring_pulse.o \
       $(BUILDDIR)/wiring_pulse.o \
       $(BUILDDIR)/wiring_shift.o \
       $(BUILDDIR)/WMath.o \
       $(BUILDDIR)/WString.o \
       $(BUILDDIR)/HID.o \
       $(BUILDDIR)/Keyboard.o

all: debug

build/main-$(BUILD).hex: $(BUILDDIR) $(BUILDDIR)/main.hex
	cp $(BUILDDIR)/main.hex $@

release:
	$(MAKE) BUILD=release BUILDDIR=$(BUILDDIR)/release build/main-release.hex

debug:
	$(MAKE) BUILD=debug CPPFLAGS=-DDEBUG BUILDDIR=$(BUILDDIR)/debug build/main-debug.hex

attach:
	screen $(PORT) 9600

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.S
	$(CC) -c -g -x assembler-with-cpp -flto -MMD -mmcu=$(MCU) $(_CPPFLAGS) $^ -o $@

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(_CXXFLAGS) $(_CPPFLAGS) $^ -o $@

$(BUILDDIR)/%.o: %.c
	$(CXX) $(_CFLAGS) $(_CPPFLAGS) $^ -o $@

$(BUILDDIR)/main.elf: $(CORE)
	$(CC) -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=atmega32u4 \
		-o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILDDIR)/main.eep: $(BUILDDIR)/main.elf
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load \
		--no-change-warnings --change-section-lma .eeprom=0 $^ $@

$(BUILDDIR)/main.hex: $(BUILDDIR)/main.elf $(BUILDDIR)/main.eep
	avr-objcopy -O ihex -R .eeprom $< $@

size: $(BUILDDIR)/main.elf
	avr-size -A $^

burn-debug:
	$(MAKE) BUILD=debug BUILDDIR=$(BUILDDIR)/debug burn

burn-release:
	$(MAKE) BUILD=release BUILDDIR=$(BUILDDIR)/release burn

burn: $(BUILDDIR)/main.hex
	python reset.py $(PORT)
	sleep 2
	avrdude -C /etc/avrdude.conf -v -patmega32u4 -cavr109 -P$(PORT) -b57600 -D -Uflash:w:$<:i

tags:
	ctags *.c *.cpp *.h
