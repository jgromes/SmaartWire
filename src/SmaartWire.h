#ifndef _SMAART_WIRE_H
#define _SMAART_WIRE_H

/*
  Based on the OneWire library for Arduino:
  https://github.com/PaulStoffregen/OneWire
  by Paul Stoffregen et al.
  Under the following license:
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#if defined(__AVR__)
  #define PIN_TO_BASEREG(pin)                   (portInputRegister(digitalPinToPort(pin)))
  #define PIN_TO_BITMASK(pin)                   (digitalPinToBitMask(pin))
  #define IO_REG_TYPE                           uint8_t
  #define IO_REG_BASE_ATTR                      asm("r30")
  #define IO_REG_MASK_ATTR
  #define DIRECT_READ(base, mask)               (((*(base)) & (mask)) ? 1 : 0)
  #define DIRECT_MODE_INPUT(base, mask)         ((*((base)+1)) &= ~(mask))
  #define DIRECT_MODE_INPUT_PULLUP(base, mask)  ((*((base)+1)) &= ~(mask))
  #define DIRECT_MODE_OUTPUT(base, mask)        ((*((base)+1)) |= (mask))
  #define DIRECT_WRITE_LOW(base, mask)          ((*((base)+2)) &= ~(mask))
  #define DIRECT_WRITE_HIGH(base, mask)         ((*((base)+2)) |= (mask))
#else
  #error "Sorry, this architecture is not supported by SmaartWire library."
#endif

/*!
  \class SmaartWire
  
  \brief Main library object to control the SmaartWire interface.
 */
class SmaartWire {
  public:
    //! The default constructor.
    /*!
      \param pin Digital I/O pin that will be used to connect to the SmaartWire device.
    */
    SmaartWire(int pin);
    
    //! Start communication with the SmaartWire device.
    /*!
      \param speed Baud rate to use.
    */
    void begin(long speed);
    
    //! Function to write single byte to the device
    /*!
      \param b Single byte to write.
    */
    void write(uint8_t b);
    
    //! Function to read single byte from the device
    /*!
      \return Returns single byte read from device.
    */
    uint8_t read();
  
  private:
    uint16_t _brDelay;
    IO_REG_TYPE _bitMask;
    volatile IO_REG_TYPE * _baseReg;
    
    void writeBit(uint8_t b);
    uint8_t readBit();
};

#endif
