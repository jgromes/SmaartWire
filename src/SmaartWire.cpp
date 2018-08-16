#include "SmaartWire.h"

SmaartWire::SmaartWire(int pin) {
  pinMode(pin, INPUT);
  _bitMask = PIN_TO_BITMASK(pin);
	_baseReg = PIN_TO_BASEREG(pin);
}

void SmaartWire::begin(long speed) {
  _brDelay = 1000000/speed;
}

void SmaartWire::write(uint8_t b) {
  uint8_t bitMask;
  
  // start pulse
  noInterrupts();
  DIRECT_MODE_OUTPUT(_baseReg, _bitMask);
  DIRECT_WRITE_LOW(_baseReg, _bitMask);
  delayMicroseconds(_brDelay);
  interrupts();
  
  // write bits
  for (bitMask = 0x01; bitMask; bitMask <<= 1) {
    writeBit((bitMask & b) ? 1: 0);
  }
  
  // stop pulse
	noInterrupts();
  DIRECT_WRITE_HIGH(_baseReg, _bitMask);
  delayMicroseconds(_brDelay);
	DIRECT_MODE_INPUT(_baseReg, _bitMask);
	DIRECT_WRITE_LOW(_baseReg, _bitMask);
	interrupts();
}

uint8_t SmaartWire::read() {
  uint8_t bitMask;
  uint8_t r = 0;
  
  uint32_t start = millis();
  while((DIRECT_READ(_baseReg, _bitMask) == 1) && (millis() - start < _brDelay * 10));
  delayMicroseconds(_brDelay + _brDelay/2);
  
  for (bitMask = 0x01; bitMask; bitMask <<= 1) {
    if (readBit()) {
      r |= bitMask;
    }
  }
  
  return r;
}

void SmaartWire::writeBit(uint8_t b) {
  IO_REG_TYPE mask IO_REG_MASK_ATTR = _bitMask;
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = _baseReg;

	if (b & 1) {
		noInterrupts();
		DIRECT_MODE_OUTPUT(reg, mask);
		DIRECT_WRITE_HIGH(reg, mask);
		interrupts();
		delayMicroseconds(_brDelay);
	} else {
		noInterrupts();
		DIRECT_WRITE_LOW(reg, mask);
		DIRECT_MODE_OUTPUT(reg, mask);
    interrupts();
		delayMicroseconds(_brDelay);
  }
}

uint8_t SmaartWire::readBit() {
	IO_REG_TYPE mask IO_REG_MASK_ATTR = _bitMask;
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = _baseReg;
	uint8_t r;

	noInterrupts();
	DIRECT_MODE_INPUT(reg, mask);
	r = DIRECT_READ(reg, mask);
	interrupts();
	delayMicroseconds(_brDelay);
	return r;
}
