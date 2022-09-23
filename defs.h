#if !defined(DEFS_H)
#    define DEFS_H

#    include <Arduino.h>

#    if defined(ARDUINO_SAM_DUE)
// arduino due defs
#        define CONSOLE SERIAL_PORT_USBVIRTUAL
#        define SERIAL1 SERIAL_PORT_HARDWARE_OPEN
#        define SERIAL2 SERIAL_PORT_HARDWARE_OPEN1
#        define SERIAL3 SERIAL_PORT_HARDWARE_OPEN2

#        define SERIAL_US SERIAL2

#    elif defined(ARDUINO_AVR_UNO)
// arduino uno defs
#        define CONSOLE SERIAL_PORT_MONITOR
#        define SERIAL1 SERIAL_PORT_HARDWARE
// there is only 1 serial on the uno

#        define SERIAL_US SERIAL1

#    elif defined(ARDUINO_AVR_LEONARDO)
// arduino leonardo defs
#        define CONSOLE SERIAL_PORT_MONITOR
#        define SERIAL1 SERIAL_PORT_HARDWARE
// there is only 1 serial on the uno

#        define SERIAL_US SERIAL1

#    else
#        error Unknown Board
#    endif

template<typename T>
T &&move(T &v)
{
    return static_cast<T &&>(v);
}

#endif // defined(DEFS_H)
