
cmake_minimum_required(VERSION 2.8)

add_subdirectory("tools")       #some extra tools

include("to_install_dir/avrCppLib.cmake")

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
    MODULE_HEADERS src/common.hpp src/datatypes.h src/delay.hpp src/lib.h src/lib.hpp src/fixed.hpp src/progmem.hpp src/cputype.hpp
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
                    src/devices/twi.hpp
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

avr_module(ds18b20
    MODULE_HEADERS src/hardware/thermometers/ds18b20.hpp
    MODULE_MODE library
    MODULE_INC_PATH include/hardware/thermometers
)

avr_module(protocols
    #MODULE_SOURCES src/protocols/one_wire.cpp
    MODULE_HEADERS src/protocols/one_wire.hpp
    MODULE_MODE library
    MODULE_INC_PATH include/protocols/
)

avr_module(interrupts
    MODULE_SOURCES src/interrupts/pcint.cpp
    MODULE_HEADERS src/interrupts/interrupts.hpp src/interrupts/pcint.hpp
    MODULE_MODE library
    MODULE_INC_PATH include/interrupts/
)

avr_module(utils
    MODULE_SOURCES src/utils/waitcondition.cpp
    MODULE_HEADERS src/utils/waitcondition.hpp src/utils/macros.hpp
    MODULE_MODE library
    MODULE_INC_PATH include/utils/
)

install (DIRECTORY to_install_dir/cpus
		 DESTINATION ${BASE_DIR}/share/avrCppLib
		 FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
		 DIRECTORY_PERMISSIONS  OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
		 
install (FILES to_install_dir/info to_install_dir/avrCppLib.cmake
	     DESTINATION ${BASE_DIR}/share/avrCppLib
	     PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

configure_file(avr_cpp_lib.cmake.in "${PROJECT_BINARY_DIR}/avrCppLibConfig.cmake" @ONLY)

install(FILES "${PROJECT_BINARY_DIR}/avrCppLibConfig.cmake" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/avrCppLib/)


#tests
include_directories(./src)
add_executable(tests src/tests.cpp)
target_link_libraries(tests CppUTest)
set_target_properties(tests PROPERTIES COMPILE_FLAGS "-std=c++11")

get_target_property(exec tests LOCATION)

add_custom_target(runTests 
                  COMMAND ${exec}
                  DEPENDS tests)

