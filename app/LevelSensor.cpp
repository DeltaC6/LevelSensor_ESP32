/*!
 * @author  Syed Asad Amin
 * @file    LevelSensor.cpp
 * @date    March 30th, 2021
 * @version V0.0.4 -> CHANGELOG
 *              | v0.0.0 -> Added class library for level sensor
 *              | v0.0.1 -> Added hardware serial support
 *              | v0.0.2 -> Added fast CRC16/MODBUS calculation support
 *              | v0.0.3 -> Added support for probe address read and write
 *              | v0.0.4 -> Added support for probe address response packet
 *                          processing
 *              | v0.0.5 -> Added support for probe level sensor data packets
 *              | v0.0.6 -> Added getters and setters for the data packets
 * 
 * @note    This is a library for Magnetostrictive Probe - (SYWA) written in
 *          C++ on ESP32 Arduino Core Framework.
 * 
 *          This library enables user to access all of sensor operations as 
 *          mentioned in the datasheet as well as allows the user to use this
 *          library on daisy chain sensor network.
 * 
 * @sa      /docs/Sensor_Datasheet.pdf
 * 
 */

#include "LevelSensor.h"

namespace LevelSensor {

    static const uint16_t table[256] PROGMEM = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    MagnetoProbe_SYWA::MagnetoProbe_SYWA() {
        memset(&data, 0, sizeof(data));
        memset(rawData, 0, sizeof(rawData));

        probeAddress = 0;
    }

    MagnetoProbe_SYWA::~MagnetoProbe_SYWA() {
        delete(modbus);
    }

    bool MagnetoProbe_SYWA::begin(void) {
        modbus = new HardwareSerial(2);
        modbus->begin(SERIAL_BAUD, SERIAL_8N1, SERIAL_RX, SERIAL_TX);
        modbus->setTimeout(1000);
        modbus->setRxBufferSize(256);

        delay(50);

        return true;
    }

    bool MagnetoProbe_SYWA::readProbeAddress(void) {
        const uint8_t packet[] = {
            broadcastAddress,                   // 0x00
            readAddress,                        // 0x04
            regSensorAddress & 0xFF,            // 0x20
            (regSensorAddress >> 8) & 0xFF,     // 0x00
            0x00,                               // 0x00
            0x01                                // 0x01
        };

        sendData(packet, sizeof(packet));
        delay(100);

        read(rawData, sizeof(rawData));
        return processData();
    }

    bool MagnetoProbe_SYWA::writeProbeAddress(const uint8_t add) {
        const uint8_t packet[] = {
            broadcastAddress,                   // 0x00
            writeAddress,                       // 0x06
            (regSensorAddress >> 8) & 0xFF,     // 0x00
            (regSensorAddress & 0xFF),          // 0x20
            0x00,                               // 0x00
            add                                 // 0x??
        };
        
        sendData(packet, sizeof(packet));
        delay(100);

        read(rawData, sizeof(rawData));
        return processData();
    }

    bool MagnetoProbe_SYWA::getData(void) {
        const uint8_t packet[] = {
            probeAddress,                       // 0x??
            readAddress,                        // 0x04
            (regFuelLevelL >> 8) & 0xFF,        // 0x00
            (regFuelLevelL & 0xFF),             // 0x00
            (noOfRegisters >> 8) & 0xFF,        // 0x00
            (noOfRegisters & 0xFF)              // 0x10
        };

        sendData(packet, sizeof(packet));
        delay(100);

        read(rawData, sizeof(rawData));
        return processData();
    }

    void MagnetoProbe_SYWA::write(const uint8_t *buf, uint32_t len) {
        rs485ModeTX();
        modbus->write(buf, len);
        delay(100);
    }

    void MagnetoProbe_SYWA::read(uint8_t *buf, uint32_t len) {
        rs485ModeRX();
        if(modbus->available()) {
            modbus->read(buf, len);
        }
        delay(100);
    }

    bool MagnetoProbe_SYWA::sendData(const uint8_t *buf, uint32_t len) {
        bool ret = false;

        uint16_t mCRC = getCRC(buf, len);
        uint32_t lenCRC = sizeof(mCRC);
        uint32_t totLen = len + lenCRC;

        uint8_t *buffer = (uint8_t *) malloc(totLen);
        memcpy(buffer, buf, len);
        memcpy(&buffer[len], &mCRC, lenCRC);

        write(buffer, totLen);

        free(buffer);
        return ret;
    }

    void MagnetoProbe_SYWA::rs485ModeTX(void) {
        digitalWrite(RS485_DE, HIGH);
        digitalWrite(RS485_RE, HIGH);
    }

    void MagnetoProbe_SYWA::rs485ModeRX(void) {
        digitalWrite(RS485_DE, LOW);
        digitalWrite(RS485_RE, LOW);
    }

    bool MagnetoProbe_SYWA::processData(void) {
        uint8_t sData[4] = {0};
        uint8_t idx = 0;

        bool validFlag = false;
        bool foundFlag = false;
        bool broadcastFlag = false;

        if(rawData[idx] == probeAddress) {
            foundFlag = true;
        } else if(rawData[idx] == broadcastAddress) {
            broadcastFlag = true;
        }

        idx++;
        if(foundFlag) {
            if(rawData[idx] == readAddress) {
                // 0x??, 0x04, 0x20, 0xNN, CRC_L, CRC_H
                idx++;
                uint32_t len = rawData[idx++];
                uint32_t n = len / sizeof(float);
                for(uint32_t i = 0; i < n; i++) {
                    sData[2] = rawData[idx++];
                    sData[3] = rawData[idx++];
                    sData[0] = rawData[idx++];
                    sData[1] = rawData[idx++];
                    memcpy(&data.values[i], sData, sizeof(float));
                }
                uint16_t mCRC = getCRC(rawData, idx);
                uint16_t pCRC = rawData[idx] + (rawData[idx+1] << 8);
                if(mCRC == pCRC) { validFlag = true; }
            }
        }
        if(broadcastFlag) {
            if(rawData[idx] == readAddress) {
                // 0x00, 0x04, 0x02, 0x00, 0x07, CRC_L, CRC_H
                idx++;
                uint8_t len = rawData[idx];
                for(int i = 0; i < len; i++) {
                    probeAddress += (rawData[idx + i] << i * 8);
                }
                idx += len;
                uint16_t mCRC = getCRC(rawData, idx);
                idx++;
                uint16_t pCRC = rawData[idx] + (rawData[idx+1] << 8);
                if(mCRC == pCRC) { validFlag = true; }
            } else if(rawData[idx] == writeAddress) {
                //0x00, 0x06, 0x00, 0x20, 0x00, 0x?? CRC_L, CRC_H
                idx++;
                uint16_t regAdd = (rawData[idx] << 8) + (rawData[idx+1]);
                if(regAdd == regSensorAddress) {
                    idx+=3;
                    probeAddress = rawData[idx];
                }
                uint16_t mCRC = getCRC(rawData, idx);
                idx++;
                uint16_t pCRC = rawData[idx] + (rawData[idx+1] << 8);
                if(mCRC == pCRC) { validFlag = true; }
            }
        }

        return validFlag;
    }

    uint16_t MagnetoProbe_SYWA::getCRC(const uint8_t *buf, uint32_t len) {
        uint8_t val = 0;
        uint16_t crc = 0xFFFF;
        while( len-- ) {
            val = *buf++ ^ crc;
            crc >>= 8;
            crc ^= table[val];
        }
        return crc;
    }

}
