/*!
 * @author  Syed Asad Amin
 * @file    app.ino
 * @date    March 30th, 2021
 * @version v0.0.0 -> CHANGELOG
 *              | v0.0.0 -> Phase 1 of the application (Sensor Readout)
 * 
 * @note    This program is written in C++ on ESP32 using Arduino Core Framework
 *          to integrate a level sensor, lcd and data logger.
 * 
 *          The level sensor "Magnetostrictive Probe" used is based on RS485
 *          communication bus.
 * 
 * @sa      /docs/Sensor_Datasheet.pdf
 */

//===== INCLUDE SECTION ======================================================//
#include "LevelSensor.h"
//============================================================================//

//===== DEFINITIONS SECTION ==================================================//
#define debug   Serial

// IO's
static const uint8_t LED_BLUE = 2;

// Timer's
static hw_timer_t *timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//============================================================================//

//===== GLOBAL VARIABLES SECTION =============================================//
LevelSensor::MagnetoProbe_SYWA sensor;
//============================================================================//

//===== ISR SECTION ==========================================================//

void IRAM_ATTR timerISR(void) {
    portENTER_CRITICAL_ISR(&timerMux);

    // Heartbeat. Blue LED should blink every second.
    static uint32_t counter = 0;
    counter++;
    if(counter > 1000) {
        counter = 0;
        digitalWrite(LED_BLUE, !digitalRead(2));
    }

    portEXIT_CRITICAL_ISR(&timerMux);
}

//============================================================================//

void setup(void) {
    InitSerial();
    InitGPIO();
    InitTimer();

    if(sensor.begin()) {
        debug.println("Level sensor initialized successfully.");
    }

    sensor.setProbeAddress(0x07);

    debug.println("Initialization complete.");
}

void loop(void) {
    sensor.getData();
    showSensorData();
    delay(1000);
}

void InitSerial(void) {
    debug.begin(115200);
    while(!debug);
}

void InitGPIO(void) {
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_BLUE, LOW);
}

void InitTimer(void) {
    timer = timerBegin(1, 80, true);
    timerAttachInterrupt(timer, &timerISR, true);
    timerAlarmWrite(timer, 1000, true);
    timerAlarmEnable(timer);
}

void showSensorData(void) {
    debug.print("Fuel Level: ");
    debug.print(sensor.getFuelLevel() / 10);
    debug.println();

    debug.print("Water Level: ");
    debug.print(sensor.getWaterLevel() / 10);
    debug.println();

    debug.print("Fuel Avg Temp: ");
    debug.print(sensor.getFuelAvgTemp());
    debug.println();
}
