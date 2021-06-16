/*!
 * @author  Syed Asad Amin
 * @file    app.ino
 * @date    March 30th, 2021
 * @version v0.0.2 -> CHANGELOG
 *              | v0.0.0 -> Phase 1 of the application (Sensor Readout)
 *              | v0.0.1 -> Phase 2 of the application (Data on LCD)
 *              | v0.0.2 -> Phase 3 of the application (Bluetooth stream)
 *              | v0.0.3 -> Phase 4 of the application (Power saving)
 * 
 * @note    This program is written in C++ on ESP32 using Arduino Core Framework
 *          to integrate a level sensor, lcd and data logger.
 * 
 *          The level sensor "Magnetostrictive Probe" used is based on RS485
 *          communication bus.
 * 
 * @sa      /docs/Sensor_Datasheet.pdf & /docs/Connection_Diagram.png
 * 
 */

//===== INCLUDE SECTION ======================================================//
#include "LevelSensor.h"
#include <LiquidCrystal_I2C.h>
#include <BluetoothSerial.h>
//============================================================================//

//===== DEFINITIONS SECTION ==================================================//
#define debug   Serial

// IO's
static const uint8_t BTN_BOOT = 0;
static const uint8_t LED_BLUE = 2;
static const uint8_t SENSOR_RELAY_PIN = 32;
static const uint8_t MODULE_RELAY_PIN = 33;

// Timer's
static hw_timer_t *timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//============================================================================//

//===== GLOBAL VARIABLES SECTION =============================================//
static const uint8_t DEB_TIME = 50;

static const uint8_t PROBE_ADDR = 0x07;
static LevelSensor::MagnetoProbe_SYWA sensor;

static const uint8_t LCD_ADDR = 0x27;
static const uint8_t LCD_COLS = 20;
static const uint8_t LCD_ROWS = 4;
static LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

static const uint8_t BT_NAME[] = "ESP32_LS";
static const uint8_t BT_PIN[] = "1234";
static BluetoothSerial bt;

// Time in seconds to turn of the sensor and devices for power saving.
static uint32_t powerSaveTime = 120;
static uint32_t relayCounter = 0;
//============================================================================//

//===== ISR SECTION ==========================================================//

void IRAM_ATTR timerISR(void) {
    portENTER_CRITICAL_ISR(&timerMux);

    // Heartbeat. Blue LED should blink every second.
    static uint32_t counter = 0;
    counter++;
    if(counter > 1000) {
        counter = 0;
        digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
    }

    relayCounter++;
    if(relayCounter >= (powerSaveTime * 1000)) {
        relayCounter = 0;
        deviceEn(0);
    }

    portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR buttonISR(void) {
    static uint32_t debounceTimer = 0;
    if(millis() - DEB_TIME > debounceTimer) {
        debounceTimer = millis();
        relayCounter = 0;
        deviceEn(1);
    }
}

//============================================================================//

void setup(void) {
    InitSerial();
    InitGPIO();
    InitTimer();
    InitLCD();
    InitBT();

    if(!sensor.begin()) {
        debug.println("Could not initialize level sensor.");
        return;
    }
    sensor.setProbeAddress(PROBE_ADDR);

    debug.println("Initialization complete.");
}

void loop(void) {
    sensor.getData();
    showSensorData();
    sendDataViaBT();
    delay(1000);
}

void InitSerial(void) {
    debug.begin(115200);
    while(!debug);
}

void InitGPIO(void) {
    pinMode(LED_BLUE, OUTPUT);
    pinMode(SENSOR_RELAY_PIN, OUTPUT);
    pinMode(MODULE_RELAY_PIN, OUTPUT);

    digitalWrite(LED_BLUE, LOW);
    digitalWrite(SENSOR_RELAY_PIN, LOW);
    digitalWrite(MODULE_RELAY_PIN, LOW);

    pinMode(BTN_BOOT, INPUT_PULLUP);
    attachInterrupt(BTN_BOOT, buttonISR, FALLING);
}

void InitTimer(void) {
    timer = timerBegin(1, 80, true);
    timerAttachInterrupt(timer, &timerISR, true);
    timerAlarmWrite(timer, 1000, true);
    timerAlarmEnable(timer);
}

void InitLCD(void) {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void InitBT(void) {
    bt.begin(String((char *) BT_NAME));
    bt.setPin((char *) BT_PIN);
}

void showSensorData(void) {
    debug.print("Fuel Level: ");
    debug.print(sensor.getFuelLevel() / 10);
    debug.println(" cm");

    debug.print("Water Level: ");
    debug.print(sensor.getWaterLevel() / 10);
    debug.println(" cm");

    debug.print("Fuel Avg Temp: ");
    debug.print(sensor.getFuelAvgTemp());
    debug.println(" C");

    lcd.setCursor(0, 0);
    lcd.print("FL: ");
    lcd.print(sensor.getFuelLevel() / 10);
    lcd.print(" cm");

    lcd.setCursor(0, 1);
    lcd.print("WL: ");
    lcd.print(sensor.getWaterLevel() / 10);
    lcd.print(" cm");

    lcd.setCursor(0, 2);
    lcd.print("FAT: ");
    lcd.print(sensor.getFuelAvgTemp());
    lcd.print(" C");
}

void sendDataViaBT(void) {
    constexpr uint16_t header = { 0xDEC0 };
    constexpr uint16_t footer = { 0xFECA };
    uint8_t buf[256];
    uint8_t len = 0;

    memcpy(buf, &header, sizeof(header));
    len += sizeof(uint16_t);

    uint8_t add = sensor.getProbeAddress();
    memcpy(&buf[len], &add, sizeof(add));
    len += sizeof(add);

    float fl = sensor.getFuelLevel();
    memcpy(&buf[len], &fl, sizeof(fl));
    len += sizeof(fl);

    float wl = sensor.getWaterLevel();
    memcpy(&buf[len], &wl, sizeof(wl));
    len += sizeof(wl);

    float t = sensor.getFuelAvgTemp();
    memcpy(&buf[len], &t, sizeof(t));
    len += sizeof(t);

    memcpy(&buf[len], &footer, sizeof(footer));
    len += sizeof(footer);

    bt.write(buf, len);
}

void deviceEn(uint8_t enable) {
    digitalWrite(SENSOR_RELAY_PIN, !enable);
    digitalWrite(MODULE_RELAY_PIN, !enable);
}
