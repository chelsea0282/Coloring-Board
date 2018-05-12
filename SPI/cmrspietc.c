// notes: Joystick Sensor SRC-8 Check sum not setup

#include "comSPI.h"

void initSPIPins() {
  mPORTDClearBits(SDI1 | SDO1);
  mPORTDSetPinsDigitalIn(SDI1);
  mPORTDSetPinsDigitalOut(SS1 | SS4 | SS5 | SDO1);
  mPORTDSetBits(SS1 | SS4 | SS5);
  mPORTCSetPinsDigitalOut(SS2 | SS3);
  mPORTCSetBits(SS2 | SS3);
  SDI1R = PPSRD3InReg;  // sets RPD3 to SDI1
  RPD4R = PPSSDO1Reg;   // Sets SDO1 to RPD4
  // I think the default idle for SS should be high but I dunno
}

void initSPIDataStruct(struct spiData* data) {
  (*data).slave1.rotation = E1RO;
  (*data).slave1.rawData = 0;
  (*data).slave1.err = 0;
  (*data).slave1.scaled = 0;
  (*data).slave2.rotation = E2RO;
  (*data).slave2.rawData = 0;
  (*data).slave2.err = 0;
  (*data).slave2.scaled = 0;
  (*data).slave3.rotation = E3RO;
  (*data).slave3.rawData = 0;
  (*data).slave3.err = 0;
  (*data).slave3.scaled = 0;
  (*data).slave4.rotation = 0;
  (*data).slave4.rawData = 0;
  (*data).slave4.err = 0;
  (*data).slave4.scaled = 0;
  (*data).slave5.rotation = 0;
  (*data).slave5.rawData = 0;
  (*data).slave5.err = 0;
  (*data).slave5.scaled = 0;
  (*data).busErr = 0;
}

uint16_t transferSPI(uint16_t data) {
  SpiChnPutC(1, data);
  return SpiChnGetC(1);
}

uint16_t transfer16SPI(uint8_t ssVal, uint16_t data) {
  // triggers SS line and returns value(returns 0xFFFF if error)
  uint16_t temp;
  switch (ssVal) {
    case 1:
      startSPIS1();
      temp = transferSPI(data);
      endSPIS1();
      return temp;
      break;
    case 2:
      startSPIS2();
      temp = transferSPI(data);
      endSPIS2();
      return temp;
      break;
    case 3:
      startSPIS3();
      temp = transferSPI(data);
      endSPIS3();
      return temp;
      break;
    case 4:
      startSPIS4();
      temp = transferSPI(data);
      endSPIS4();
      return temp;
      break;
    case 5:
      startSPIS5();
      temp = transferSPI(data);
      endSPIS5();
      return temp;
      break;
    default:
      return 0xFFFF;
      break;
  }
}

uint64_t transfer64SPI(uint8_t ssVal, uint64_t data) {
  // triggers SS line and returns value(returns 0xFFFFFFFFFFFFFFFF if error)
  uint16_t data1, data2, data3, data4;
  uint64_t temp;
  data1 = data & 0xFF;
  data2 = (data >> 16) & 0xFF;
  data3 = (data >> 32) & 0xFF;
  data4 = (data >> 48) & 0xFF;
  switch (ssVal) {
    case 1:
      startSPIS1();
      temp = transferSPI(data1);
      temp = (temp << 16) | transferSPI(data2);
      temp = (temp << 32) | transferSPI(data3);
      temp = (temp << 48) | transferSPI(data4);
      endSPIS1();
      return temp;
      break;
    case 2:
      startSPIS2();
      temp = transferSPI(data1);
      temp = (temp << 16) | transferSPI(data2);
      temp = (temp << 32) | transferSPI(data3);
      temp = (temp << 48) | transferSPI(data4);
      endSPIS2();
      return temp;
      break;
    case 3:
      startSPIS3();
      temp = transferSPI(data1);
      temp = (temp << 16) | transferSPI(data2);
      temp = (temp << 32) | transferSPI(data3);
      temp = (temp << 48) | transferSPI(data4);
      endSPIS3();
      return temp;
      break;
    case 4:
      startSPIS4();
      temp = transferSPI(data1);
      temp = (temp << 16) | transferSPI(data2);
      temp = (temp << 32) | transferSPI(data3);
      temp = (temp << 48) | transferSPI(data4);
      endSPIS4();
      return temp;
      break;
    case 5:
      startSPIS5();
      temp = transferSPI(data1);
      endSPIS5();
      return temp;
      break;
    default:
      return 0xFFFFFFFFFFFFFFFF;
      break;
  }
}

uint16_t getEncoderRotationRaw(uint8_t ssVal) {
  uint16_t angle, temp2;
  angle = transfer16SPI(ssVal, 0xFFFF);
  temp2 = angle;  // used for checking parity
  uint8_t count = 0;
  while (temp2) {  // parity checking loop Think about using a lookup table
                   // later
    temp2 &= (temp2 - 1);
    count++;
  }
  if ((angle & 0x4000) == 0x4000 ||
      (count % 2) != 0) {  // check error bit *if error send 0xFF
    return 0xFFFF;
  } else {
    return (angle & 0x3FFF);
  }
}

uint16_t getEncoderRotation(uint8_t ssVal) {
  // returns rotation in 10tenths of a degree
  uint16_t angle;
  angle = getEncoderRotationRaw(ssVal);
  if (angle == 0xFFFF) return 0xFFFF;
  return (uint16_t)(((unsigned long)angle) * 3600UL / 16384UL);
}

uint16_t getEncoderErrorCode(uint8_t ssVal) {
  uint16_t code, temp2;
  code = transfer16SPI(ssVal, 0x8001);
  temp2 = code;  // used for checking parity
  uint8_t count = 0;
  while (temp2) {  // parity checking loop Think about using a lookup table
                   // later
    temp2 &= (temp2 - 1);
    count++;
  }
  if ((code & 0x4000) == 0x4000 ||
      (count % 2) != 0) {  // check error bit *if error send 0xFF
    return 0xFFFF;
  } else {
    return (code & 0x3FFF);
  }
}

uint16_t getEncoderDiagnosticCode(uint8_t ssVal) {
  uint16_t code, temp2;
  code = transfer16SPI(ssVal, 0x3FFC);
  temp2 = code;  // used for checking parity
  uint8_t count = 0;
  while (temp2) {  // parity checking loop Think about using a lookup table
                   // later
    temp2 &= (temp2 - 1);
    count++;
  }
  if ((code & 0x4000) == 0x4000 ||
      (count % 2) != 0) {  // check error bit *if error send 0xFF
    return 0xFFFF;
  } else {
    return (code & 0x3FFF);
  }
}

void updateEncoders(struct spiData* data, uint8_t encoderSpots) {
  uint16_t tempVal;
  if ((encoderSpots >> 0) & 0x1) {
    tempVal = getEncoderRotationRaw(1);
    if (tempVal == 0xFFFF)
      (*data).slave1.err = 1;
    else {
      if (tempVal > 0x3F00 && (*data).slave1.rawData < 0x0FF0)
        (*data).slave1.rotation -= 1;
      else if ((*data).slave1.rawData > 0x3F00 && tempVal < 0x0FF0)
        (*data).slave1.rotation += 1;
      (*data).slave1.rawData = tempVal;
      (*data).slave1.scaled =
          calcScaledEncoderValue(tempVal, (*data).slave1.rotation);
    }
  }
  if ((encoderSpots >> 1) & 0x1) {
    tempVal = getEncoderRotationRaw(2);
    if (tempVal == 0xFFFF)
      (*data).slave2.err = 1;
    else {
      if (tempVal > 0x3F00 && (*data).slave2.rawData < 0x0FF0)
        (*data).slave2.rotation -= 1;
      else if ((*data).slave2.rawData > 0x3F00 && tempVal < 0x0FF0)
        (*data).slave2.rotation += 1;
      (*data).slave2.rawData = tempVal;
      (*data).slave2.scaled =
          calcScaledEncoderValue(tempVal, (*data).slave2.rotation);
    }
  }
  if ((encoderSpots >> 2) & 0x1) {
    tempVal = getEncoderRotationRaw(3);
    if (tempVal == 0xFFFF)
      (*data).slave3.err = 1;
    else {
      if (tempVal > 0x3F00 && (*data).slave3.rawData < 0x0FF0)
        (*data).slave3.rotation -= 1;
      else if ((*data).slave3.rawData > 0x3F00 && tempVal < 0x0FF0)
        (*data).slave3.rotation += 1;
      (*data).slave3.rawData = tempVal;
      (*data).slave3.scaled =
          calcScaledEncoderValue(tempVal, (*data).slave3.rotation);
    }
  }
  if ((encoderSpots >> 3) & 0x1) {
    tempVal = getEncoderRotationRaw(4);
    if (tempVal == 0xFFFF)
      (*data).slave4.err = 1;
    else {
      if (tempVal > 0x3F00 && (*data).slave4.rawData < 0x0FF0)
        (*data).slave4.rotation -= 1;
      else if ((*data).slave4.rawData > 0x3F00 && tempVal < 0x0FF0)
        (*data).slave4.rotation += 1;
      (*data).slave4.rawData = tempVal;
      (*data).slave4.scaled =
          calcScaledEncoderValue(tempVal, (*data).slave4.rotation);
    }
  }
  if ((encoderSpots >> 4) & 0x1) {
    tempVal = getEncoderRotationRaw(5);
    if (tempVal == 0xFFFF)
      (*data).slave5.err = 1;
    else {
      if (tempVal > 0x3F00 && (*data).slave5.rawData < 0x0FF0)
        (*data).slave5.rotation -= 1;
      else if ((*data).slave5.rawData > 0x3F00 && tempVal < 0x0FF0)
        (*data).slave5.rotation += 1;
      (*data).slave5.rawData = tempVal;
      (*data).slave5.scaled =
          calcScaledEncoderValue(tempVal, (*data).slave5.rotation);
    }
  }
}

int16_t calcScaledEncoderValue(uint16_t rawValue, int8_t rotations) {
  int32_t tempVal;
  tempVal = ((int32_t)rawValue) * ticksPerRotation;
  tempVal /= 16384;
  tempVal += ticksPerRotation * rotations;
  return ((int16_t)tempVal);
}
