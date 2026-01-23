/**
 * @brief Component tests for the logic implementation.
 */
#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <thread>

#include <gtest/gtest.h>

#include "driver/eeprom/stub.h"
#include "driver/gpio/stub.h"
#include "driver/serial/stub.h"
#include "driver/tempsensor/stub.h"
#include "driver/timer/stub.h"
#include "driver/watchdog/stub.h"
#include "logic/stub.h"

//! @todo Remove this #ifdef block once all stubs are implemented!


#ifdef TESTSUITE

namespace logic
{
namespace
{
/**
 * @brief Structure of mock instances.
 * 
 * @tparam EepromSize EEPROM size in bytes (default = 1024).
 */
template <std::uint16_t EepromSize = 1024U>
struct Mock final
{
    static_assert(0U < EepromSize, "EEPROM size must be greater than 0!");

    driver::gpio::Stub led;
    driver::gpio::Stub toggleButton;
    driver::gpio::Stub tempButton;

    driver::timer::Stub debounceTimer;
    driver::timer::Stub toggleTimer;
    driver::timer::Stub tempTimer;

    driver::serial::Stub serial;
    driver::watchdog::Stub watchdog;
    driver::eeprom::Stub<EepromSize> eeprom;
    driver::tempsensor::Stub tempSensor;

    std::unique_ptr<logic::Stub> logicImpl;

    Mock() noexcept
        : led{}
        , toggleButton{}
        , tempButton{}
        , debounceTimer{}
        , toggleTimer{}
        , tempTimer{}
        , serial{}
        , watchdog{}
        , eeprom{}
        , tempSensor{}
        , logicImpl{nullptr}
    {}

    ~Mock() noexcept = default;

    logic::Interface& createLogic()
    {
        logicImpl = std::make_unique<logic::Stub>(
            led, toggleButton, tempButton,
            debounceTimer, toggleTimer, tempTimer,
            serial, watchdog, eeprom, tempSensor);
        return *logicImpl;
    }

    void runSystem(const std::size_t testDuration_ms = 10U)
    {
        if (nullptr == logicImpl)
        {
            throw std::invalid_argument("Logic not initialized!\n");
        }

        bool stop{false};
        std::thread t1{runLogicThread, std::ref(*logicImpl), std::ref(stop)};
        std::thread t2{stopLogicThread, testDuration_ms, std::ref(stop)};
        t1.join();
        t2.join();
    }

private:
    static void runLogicThread(logic::Interface& logic, bool& stop) noexcept
    {
        logic.run(stop);
    }

    static void stopLogicThread(const std::size_t timeout_ms, bool& stop) noexcept
    {
        stop = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        stop = true;
    }
};
/**
 * @brief Debounce handling test.
 */
TEST(Logic, DebounceHandling)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();

    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        EXPECT_FALSE(mock.tempButton.isInterruptEnabled());
        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    {
        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_TRUE(mock.toggleButton.isInterruptEnabled());
        EXPECT_FALSE(mock.debounceTimer.isEnabled());
    }

    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }
}

/**
 * @brief Toggle handling test.
 */
TEST(Logic, ToggleHandling)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();

    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.led.read());

    // Case 1 - Press the temperature button (wrong button)
    {
        mock.tempButton.write(true);
        logic.handleButtonEvent();
        mock.tempButton.write(false);

        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }

    // Case 2 - Press the toggle button
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    // Case 3 - Toggle timer timeout → LED on
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 4 - Toggle timer timeout → LED off
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_FALSE(mock.led.read());
    }

    // Case 5 - Toggle timer timeout → LED on
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 6 - Press toggle button again → disable toggle + LED
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_FALSE(mock.toggleTimer.isEnabled());
        EXPECT_FALSE(mock.led.read());
    }
}

/**
 * @brief Temperature handling test.
 */
TEST(Logic, TempHandling)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();

    // Establish baseline after startup
    std::size_t txCount = mock.serial.txCount();

    // Set the temperature to 25 degrees Celsius.
    mock.tempSensor.setTemperature(25);

    // Case 1 - Press the toggle button, simulate button event.
    // Expect NO temperature-related print (but other prints may occur).
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        // Update baseline to current state
        txCount = mock.serial.txCount();
    }

    // Case 2 - Press the temperature button.
    // Expect the temperature to be printed once.
    {
        mock.tempButton.write(true);
        logic.handleButtonEvent();
        mock.tempButton.write(false);

        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_EQ(mock.serial.txCount(), txCount + 1U);
        txCount = mock.serial.txCount();

        EXPECT_TRUE(mock.tempTimer.isEnabled());
    }

    // Case 3 - Simulate temperature timer timeout.
    // Expect the temperature to be printed once more.
    {
        mock.tempTimer.setTimedOut(true);
        logic.handleTempTimerTimeout();

        EXPECT_EQ(mock.serial.txCount(), txCount + 1U);
    }
}
/**
 * @brief EEPROM handling test.
 */
TEST(Logic, Eeprom)
{
    {
        Mock mock{};
        mock.createLogic();
        mock.runSystem();

        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }

    {
        Mock mock{};
        mock.eeprom.writeByte(0U, 1U);

        mock.createLogic();
        mock.runSystem();

        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }
}

} // namespace
} // namespace logic

#endif /** TESTSUITE */

//! @todo Remove this #endif once all stubs are implemented!

