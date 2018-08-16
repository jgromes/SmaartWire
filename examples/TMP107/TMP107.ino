#define SMAARTWIRE_CALIBRATION_SEQ        0x55

// TMP107 register map
#define TMP107_REG_TEMPERATURE            0x00
#define TMP107_REG_CONFIGURATION          0x01
#define TMP107_REG_HIGH_LIMIT_1           0x02
#define TMP107_REG_LOW_LIMIT_1            0x03
#define TMP107_REG_HIGH_LIMIT_2           0x04
#define TMP107_REG_LOW_LIMIT_2            0x05
#define TMP107_REG_EEPROM_1               0x06
#define TMP107_REG_EEPROM_2               0x07
#define TMP107_REG_EEPROM_3               0x08
#define TMP107_REG_EEPROM_4               0x09
#define TMP107_REG_EEPROM_5               0x0A
#define TMP107_REG_EEPROM_6               0x0B
#define TMP107_REG_EEPROM_7               0x0C
#define TMP107_REG_EEPROM_8               0x0D
#define TMP107_REG_DIE_ID                 0x0F

//TMP107_REG_CONFIGURATION
#define TMP107_CONV_RATE_15_MS            0b00000000
#define TMP107_CONV_RATE_50_MS            0b00100000
#define TMP107_CONV_RATE_100_MS           0b01000000
#define TMP107_CONV_RATE_250_MS           0b01100000
#define TMP107_CONV_RATE_500_MS           0b10000000
#define TMP107_CONV_RATE_1_S              0b10100000
#define TMP107_CONV_RATE_4_S              0b11000000
#define TMP107_CONV_RATE_16_S             0b11100000

// TMP107 commands
#define TMP107_CMD_ADDRESS_INIT           0x95
#define TMP107_CMD_ADDRESS_ASSIGN         0x0D
#define TMP107_CMD_LAST_DEVICE_POLL       0x57
#define TMP107_CMD_GLOBAL_SOFT_RST        0x5D
#define TMP107_CMD_GLOBAL_ALERT_CLR_1     0xB5
#define TMP107_CMD_GLOBAL_ALERT_CLR_2     0x75
#define TMP107_CMD_GLOBAL_READ            0b00000011
#define TMP107_CMD_GLOBAL_WRITE           0b00000001
#define TMP107_CMD_INDIVIDUAL_READ        0b00000010
#define TMP107_CMD_INDIVIDUAL_WRITE       0b00000000
#define TMP107_CMD_PTR_PADDING            0b10100000

// number of sensors in daisy chain
#define NUMBER_OF_SENSORS                 2

// include the library
#include <SmaartWire.h>

// the first TMP107 is connected to Arduino pin 4
// IMPORTANT: pull-up resistor MUST be used on the line!
//            the recommended value is 4.1k
//            (4.7k should be fine)
SmaartWire tmp107(4);

// array to hold sensor addresses
byte addr[NUMBER_OF_SENSORS];

// function to iniaitalize the sensor chain
// IMPORTANT: this function MUST be run before any measurements!
void addrInit(byte* addr) {
  // calibration phase
  tmp107.write(SMAARTWIRE_CALIBRATION_SEQ);

  // command phase
  tmp107.write(TMP107_CMD_ADDRESS_INIT);
  
  // address assign byte
  tmp107.write(TMP107_CMD_ADDRESS_ASSIGN);
  
  // data phase
  for(int i = 0; i < NUMBER_OF_SENSORS; i++) {
    addr[i] = (tmp107.read() & 0b11111000) >> 3;
  }

  // wait until address initialization is complete
  delay(1250);
}

// function the read temperature from sensor at a given address
float getTemp(byte addr) {
  // calibration phase
  tmp107.write(SMAARTWIRE_CALIBRATION_SEQ);

  // command and address phase
  tmp107.write(TMP107_CMD_INDIVIDUAL_READ | (addr << 3));

  // register pointer phase
  tmp107.write(TMP107_REG_TEMPERATURE | TMP107_CMD_PTR_PADDING);

  // data phase
  byte tmpLSB = tmp107.read();
  byte tmpMSB = tmp107.read();

  // convert raw 14-bit number to temperature
  int raw;
  float temp;
  if(tmpMSB & 0b10000000) {
    // temperature is negative, convert it and output
    raw = ~((tmpMSB << 6) | (tmpLSB >> 2)) + 1;
    raw &= 0x3FFF;
    temp = (float)raw * -0.015625;
  } else {
    // temperature is positive, just output
    raw = (tmpMSB << 6) | (tmpLSB >> 2);
    temp = (float)raw * 0.015625;
  }
  
  return(temp);
}

// function to set measurement interval
void setInterval(byte lastAddr, byte convRate) {
  // calibration phase
  tmp107.write(SMAARTWIRE_CALIBRATION_SEQ);

  // command and address phase
  tmp107.write(TMP107_CMD_GLOBAL_WRITE | (lastAddr << 3));

  // register pointer phase
  tmp107.write(TMP107_REG_CONFIGURATION | TMP107_CMD_PTR_PADDING);

  // data phase
  tmp107.write(0x00);
  tmp107.write(convRate);
}

void setup() {
  Serial.begin(9600);
  Serial.println();

  // initialize SmaartWire interface at 9600 baud
  tmp107.begin(9600);

  // initialize sensors
  Serial.println("Initializing sensors");
  addrInit(addr);

  // print the sensor addresses
  for(int i = 0; i < NUMBER_OF_SENSORS; i++) {
    char str[23];
    sprintf(str, "Sensor %d address: 0x%02x", i, addr[i]);
    Serial.println(str);
  }

  // set measurement intervals of all sensors to 100 ms
  setInterval(addr[NUMBER_OF_SENSORS - 1], TMP107_CONV_RATE_100_MS);
}

void loop() {
  // get temperature from each sensor and calculate the average
  float average = 0;
  for(int i = 0; i < NUMBER_OF_SENSORS; i++) {
    average += getTemp(addr[i]);
  }
  average /= (float)NUMBER_OF_SENSORS;

  // print the average temperature
  Serial.println(average);

  // wait before the next measurement
  delay(100);
}
