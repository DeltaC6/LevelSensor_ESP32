/*!
 * @author  Syed Asad Amin
 * @file    LevelSensor.h
 * @date    March 30th, 2021
 * @version V0.0.7 -> CHANGELOG
 *              | v0.0.0 -> Added class library for level sensor
 *              | v0.0.1 -> Added hardware serial support
 *              | v0.0.2 -> Added fast CRC16/MODBUS calculation support
 *              | v0.0.3 -> Added support for probe address read and write
 *              | v0.0.4 -> Added support for probe address response packet
 *                          processing
 *              | v0.0.5 -> Added support for probe level sensor data packets
 *              | v0.0.6 -> Added getters and setters for the data packets
 *              | v0.0.7 -> Added debugging messages
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

#pragma once

#ifndef __LEVEL_SENSOR_H__
#define __LEVEL_SENSOR_H__

#include <Arduino.h>
#include <HardwareSerial.h>

#include <stdio.h>
#include <string.h>

#include "LevelSensor.h"

namespace LevelSensor {

    class MagnetoProbe_SYWA {
        public:
            /*!
             * @brief Default constructor
             */
            MagnetoProbe_SYWA();

            /*!
             * @brief Default destructor
             */
            virtual ~MagnetoProbe_SYWA();

            /*!
             * @brief Initialization function for the sensor
             * @param None
             * @return [bool] true if success
             */
            bool begin(void);

            /*!
             * @brief Reads the probe address of the connected sensor
             * @param None
             * @return [bool] true if success
             */
            bool readProbeAddress(void);

            /*!
             * @brief Writes the probe address to the connected sensor
             * @param [uint8_t] Probe address to be set
             * @return [bool] true if success
             */
            bool writeProbeAddress(const uint8_t add);

            /*!
             * @brief Gets the sensor reading by sending valid query packet frame
             * @param None
             * @return [bool] true if success
             */
            bool getData(void);

            // GETTERS

            inline uint8_t getProbeAddress(void) const { return probeAddress; }

            inline float getFuelLevel(void) const { return data.value.fuelLevel; }
            inline float getWaterLevel(void) const { return data.value.waterLevel; }
            inline float getFuelAvgTemp(void) const { return data.value.fuelAvgTemp; }
            inline float getAPointTemp(void) const { return data.value.aPointTemp; }
            inline float getBPointTemp(void) const { return data.value.bPointTemp; }
            inline float getCPointTemp(void) const { return data.value.cPointTemp; }
            inline float getDPointTemp(void) const { return data.value.dPointTemp; }
            inline float getEPointTemp(void) const { return data.value.ePointTemp; }

            // SETTERS

            inline void setProbeAddress(const uint8_t add) { probeAddress = add; };

            inline void setFuelLevel(const float val) { data.value.fuelLevel = val; }
            inline void setWaterLevel(const float val) { data.value.waterLevel = val; }
            inline void setFuelAvgTemp(const float val) { data.value.fuelAvgTemp = val; }
            inline void setAPointTemp(const float val) { data.value.aPointTemp = val; }
            inline void setBPointTemp(const float val) { data.value.bPointTemp = val; }
            inline void setCPointTemp(const float val) { data.value.cPointTemp = val; }
            inline void setDPointTemp(const float val) { data.value.dPointTemp = val; }
            inline void setEPointTemp(const float val) { data.value.ePointTemp = val; }

        protected:
            /*!
             * @brief Writes the data to serial
             * @param [uint8_t] Buffer to write on serial
             * @param [uint32_t] No of character to write
             * @return None
             */
            void write(const uint8_t *buf, uint32_t len);

            /*!
             * @brief Reads the data from serial
             * @param [uint8_t] Buffer to read from serial
             * @param [uint32_t] No of characters to read
             * @return None
             */
            void read(uint8_t *buf, uint32_t len);

            /*!
             * @brief Creates a valid packet frame to query the sensor
             * @param [uint8_t] Buffer to write on serial
             * @param [uint32_t] No of character to write
             * @return None
             */
            bool sendData(const uint8_t *buf, uint32_t len);

            /*!
             * @brief Sets the RS485 module on transimit mode
             * @param None
             * @return None
             */
            void rs485ModeTX(void);
            
            /*!
             * @brief Sets the RS485 module on receive mode
             * @param None
             * @return None
             */
            void rs485ModeRX(void);

            /*!
             * @brief Process the raw data of level sensor
             * @param None
             * @return [bool] true if success
             */
            bool processData(void);

            /*!
             * @brief Calculates the modbus CRC
             * @param [uint8_t] Buffer for which CRC is needed
             * @param [uint32_t] Length of buffer
             * @return [uint16_t] Modbus CRC16 value
             */ 
            uint16_t getCRC(const uint8_t *buf, uint32_t len);

        private:
            HardwareSerial *modbus;

            static const uint32_t SERIAL_BAUD       = 9600;
            static const uint8_t SERIAL_RX          = 16;
            static const uint8_t SERIAL_TX          = 17;
            static const uint8_t RS485_DE           = 26;
            static const uint8_t RS485_RE           = 25;

            uint8_t rawData[40];
            union {
                float values[8];
                struct {
                    float fuelLevel;
                    float waterLevel;
                    float fuelAvgTemp;
                    float aPointTemp;
                    float bPointTemp;
                    float cPointTemp;
                    float dPointTemp;
                    float ePointTemp;
                } value;
            } data;

            uint8_t probeAddress;
            static const uint8_t broadcastAddress   = 0x00;
            static const uint8_t readAddress        = 0x04;
            static const uint8_t writeAddress       = 0x06;
            
            static const uint16_t noOfRegisters     = 0x0010;
            static const uint16_t regSensorAddress  = 0x0020;
            static const uint16_t regFuelLevelL     = 0x0000;
            static const uint16_t regFuelLevelH     = 0x0001;
            static const uint16_t regWaterLevelL    = 0x0002;
            static const uint16_t regWaterLevelH    = 0x0003;
            static const uint16_t regFuelAvgTempL   = 0x0004;
            static const uint16_t regFuelAvgTempH   = 0x0005;
            static const uint16_t regAPointTempL    = 0x0006;
            static const uint16_t regAPointTempH    = 0x0007;
            static const uint16_t regBPointTempL    = 0x0008;
            static const uint16_t regBPointTempH    = 0x0009;
            static const uint16_t regCPointTempL    = 0x000A;
            static const uint16_t regCPointTempH    = 0x000B;
            static const uint16_t regDPointTempL    = 0x000C;
            static const uint16_t regDPointTempH    = 0x000D;
            static const uint16_t regEPointTempL    = 0x000E;
            static const uint16_t regEPointTempH    = 0x000F;
    };

}

#endif
