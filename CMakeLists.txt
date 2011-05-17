cmake_minimum_required(VERSION 2.8)

add_subdirectory("tools")       #some extra tools

include ("/home/michal/projects/root/usr/avr/CMakeLists.txt")

avr_module(display
  MODULE_HEADERS src/lcd/display.hpp src/lcd/font.h
  MODULE_MODE library
  MODULE_INC_PATH include/lcd
)

avr_module(led_7seg
  MODULE_HEADERS src/lcd/7seg/_7seg.hpp
  MODULE_MODE library
  MODULE_INC_PATH include/lcd/7seg
)

avr_module(sed1520
  MODULE_HEADERS src/lcd/sed1520/SED1520.hpp
  MODULE_INC_PATH include/lcd/sed1520
  MODULE_MODE library
)

avr_module(ks0108
  MODULE_HEADERS src/lcd/ks0108/KS0108.hpp
  MODULE_INC_PATH include/lcd/ks0108
  MODULE_MODE library
)

avr_module(hd44780
  MODULE_HEADERS src/lcd/HD44780U/HD44780.hpp
  MODULE_INC_PATH include/lcd/HD44780
  MODULE_MODE library
)

avr_module(common
	MODULE_SOURCES src/common.cpp src/fixed.cpp
  MODULE_HEADERS src/common.hpp src/datatypes.h src/delay.hpp src/lib.h src/lib.hpp src/fixed.hpp src/progmem.hpp
  MODULE_MODE library
)

avr_module(timer
  MODULE_HEADERS src/timer.hpp
  MODULE_MODE library
)

avr_module(ports
  MODULE_HEADERS src/ports/ports.hpp src/ports/ports_defs.hpp
  MODULE_MODE library
  MODULE_INC_PATH include/ports
)

avr_module(keypads
  MODULE_HEADERS src/keypad/keypad12.hpp
  MODULE_MODE library
  MODULE_INC_PATH include/keypad
)

avr_module(devices
  MODULE_HEADERS  src/devices/spi.hpp 
                  src/devices/baudrate.hpp 
                  src/devices/usart.hpp 
                  src/devices/usartbase.hpp
  MODULE_MODE library
  MODULE_INC_PATH include/devices
)

avr_module(rfm12
  MODULE_HEADERS src/hardware/wireless/RFM12.hpp src/hardware/wireless/RFM12BP.hpp
  MODULE_MODE library
  MODULE_INC_PATH include/hardware/wireless
)
