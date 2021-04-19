/*!
 * @author  Syed Asad Amin
 * @file    LevelSensor.h
 * @date    March 30th, 2021
 * @version V0.0.0 -> CHANGELOG
 *              | v0.0.0 -> Added class library for level sensor.
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

        protected:

        private:
            HardwareSerial *modbus;
    };

}

#endif
