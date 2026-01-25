#include <Arduino.h>

#undef B0
#undef B1

#include "driver/adc/atmega328p.h"
#include "driver/eeprom/atmega328p.h"
#include "driver/gpio/atmega328p.h"
#include "driver/serial/atmega328p.h"
#include "driver/tempsensor/tmp36.h"
#include "driver/timer/atmega328p.h"
#include "driver/watchdog/atmega328p.h"
#include "logic/logic.h"

using namespace driver;

// --- GLOBAL VARIABLES ---
namespace {
    logic::Interface* myLogic{nullptr};

    namespace callback {
        void button() noexcept { if(myLogic) myLogic->handleButtonEvent(); }
        void debounceTimer() noexcept { if(myLogic) myLogic->handleDebounceTimerTimeout(); }
        void toggleTimer() noexcept { if(myLogic) myLogic->handleToggleTimerTimeout(); }
        void tempTimer() noexcept { if(myLogic) myLogic->handleTempTimerTimeout(); }
    } 
}

// --- ARDUINO SETUP ---
void setup() {
    Serial.begin(9600U);

    constexpr uint8_t tempSensorPin{2U};
    constexpr uint8_t ledPin{9U};
    constexpr uint8_t toggleButtonPin{4U};
    constexpr uint8_t tempButtonPin{7U};

    constexpr uint32_t debounceTimerTimeout{300U};
    constexpr uint32_t toggleTimerTimeout{100U};
    constexpr uint32_t tempTimerTimeout{60000U};

    constexpr auto input{gpio::Direction::InputPullup};
    constexpr auto output{gpio::Direction::Output};

    // We use 'static' so these objects live forever in memory
    static gpio::Atmega328p led{ledPin, output};
    static gpio::Atmega328p toggleButton{toggleButtonPin, input, callback::button};
    static gpio::Atmega328p tempButton{tempButtonPin, input, callback::button};

    static timer::Atmega328p debounceTimer{debounceTimerTimeout, callback::debounceTimer};
    static timer::Atmega328p toggleTimer{toggleTimerTimeout, callback::toggleTimer};
    static timer::Atmega328p tempTimer{tempTimerTimeout, callback::tempTimer};

    auto& serial{serial::Atmega328p::getInstance()};
    auto& watchdog{watchdog::Atmega328p::getInstance()};
    auto& eeprom{eeprom::Atmega328p::getInstance()};
    auto& adc{adc::Atmega328p::getInstance()};

    static tempsensor::Tmp36 tempSensor{tempSensorPin, adc};

    static logic::Logic logic{led, toggleButton, tempButton, debounceTimer, 
                               toggleTimer, tempTimer, serial, watchdog, 
                               eeprom, tempSensor};
    myLogic = &logic;
}

// --- ARDUINO LOOP ---
void loop() {
    if(myLogic) {
        const bool stop{false};
        myLogic->run(stop);
    }
}