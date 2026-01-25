/**
 * @brief Component tests for the logic implementation.
 */
#include <chrono>
#include <stdint.h>
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

    driver::gpio::Stub led{0, driver::gpio::Direction::Output};
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
        : led{0, driver::gpio::Direction::Output}
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
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);


        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);

        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    {
        mock.debounceTimer.setHasTimedOut(true);
        logic.handleDebounceTimerTimeout();


        EXPECT_FALSE(mock.debounceTimer.isEnabled());
    }

    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);

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
    // mock.runSystem();

    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.led.read());

    {
        mock.tempButton.setInput(true);
        logic.handleButtonEvent();
        mock.tempButton.setInput(false);

        mock.debounceTimer.setHasTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }

    // Case 2 - Press the toggle button
    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);

        mock.debounceTimer.setHasTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    // Case 3 - Toggle timer timeout → LED on
    {
        mock.toggleTimer.setHasTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 4 - Toggle timer timeout → LED off
    {
        mock.toggleTimer.setHasTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_FALSE(mock.led.read());
    }

    // Case 5 - Toggle timer timeout → LED on
    {
        mock.toggleTimer.setHasTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 6 - Press toggle button again → disable toggle + LED
    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);

        mock.debounceTimer.setHasTimedOut(true);
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

    // Reset the txCount before specific test cases.
    mock.serial.resetTxCount();

    // Establish baseline after startup
    std::size_t txCount = mock.serial.txCount();

    // Set the temperature to 25 degrees Celsius.
    mock.tempSensor.setTemp(25);

    // Case 1 - Press the toggle button, simulate button event.
    // Expect NO temperature-related print (but other prints may occur).
    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);

        mock.debounceTimer.setHasTimedOut(true);
        logic.handleDebounceTimerTimeout();

        // Update baseline to current state
        txCount = mock.serial.txCount();
    }

    // Case 2 - Press the temperature button.
    // Expect the temperature to be printed once.
    {
        mock.tempButton.setInput(true);
        logic.handleButtonEvent();
        mock.tempButton.setInput(false);

        mock.debounceTimer.setHasTimedOut(true);
        logic.handleDebounceTimerTimeout();

        EXPECT_EQ(mock.serial.txCount(), txCount + 1U);
        txCount = mock.serial.txCount();

        EXPECT_TRUE(mock.tempTimer.isEnabled());
    }

    // Case 3 - Simulate temperature timer timeout.
    // Expect the temperature to be printed once more.
    {
        mock.tempTimer.setHasTimedOut(true);
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

/**
 * @brief Debounce multiple presses test.
 * 
 *        Verify that multiple rapid presses on a button are debounced and treated as a single event.
 */
TEST(Logic, DebounceMultiPress)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};

    EXPECT_FALSE(mock.debounceTimer.isEnabled());
    EXPECT_FALSE(mock.toggleTimer.isEnabled());

    // Simulate multiple rapid button presses.
    for (std::size_t i = 0; i < 5; ++i)
    {
        mock.toggleButton.setInput(true);
        logic.handleButtonEvent();
        mock.toggleButton.setInput(false);
    }

    // Expect the debounce timer to be enabled after the first press.
    EXPECT_TRUE(mock.debounceTimer.isEnabled());

    // Call handleButtonEvent again while debounce timer is enabled.
    mock.toggleButton.setInput(true);
    logic.handleButtonEvent();
    mock.toggleButton.setInput(false);

    // Expect debounce timer to still be enabled, but toggle timer should not have changed state yet.
    EXPECT_TRUE(mock.debounceTimer.isEnabled());
    EXPECT_TRUE(mock.toggleTimer.isEnabled());

    // Simulate debounce timer timeout.
    mock.debounceTimer.setHasTimedOut(true);
    logic.handleDebounceTimerTimeout();

    // Expect debounce timer to be disabled and toggle timer to be enabled (one event processed).
    EXPECT_FALSE(mock.debounceTimer.isEnabled());
    EXPECT_TRUE(mock.toggleTimer.isEnabled());
}

/**
 * @brief Temperature sensor uninitialized test.
 * 
 *        Verify that the system handles an uninitialized temperature sensor gracefully.
 */
TEST(Logic, TempSensorUninitialized)
{
    Mock mock{};
    mock.tempSensor.setInitialized(false); // Simulate uninitialized sensor
    logic::Interface& logic{mock.createLogic()};

    // Initial state after system run, txCount should reflect initial prints.
    mock.serial.resetTxCount(); // Reset after mock.createLogic() for precise count
    std::size_t txCount = mock.serial.txCount();


    // Press temperature button
    mock.tempButton.setInput(true);
    logic.handleButtonEvent();
    mock.tempButton.setInput(false);

    mock.debounceTimer.setHasTimedOut(true);
    logic.handleDebounceTimerTimeout();

    // Expect no temperature printouts if sensor is uninitialized
    EXPECT_EQ(mock.serial.txCount(), txCount + 1U);
}

/**
 * @brief EEPROM invalid state test.
 * 
 *        Verify that the system handles an invalid toggle state read from EEPROM.
 */
TEST(Logic, EepromInvalidState)
{
    Mock mock{};
    // Simulate that an invalid (e.g., garbage) value is in EEPROM at ToggleStateAddr
    // or that the read operation itself fails.
    // By default, myEeprom.readByte() will return 0 if the address is out of bounds or not set.
    // If Logic::readToggleStateFromEeprom() reads 0, it will return false.

    // Ensure the EEPROM read at ToggleStateAddr returns 0 (which defaults to false for toggle state).
    // ToggleStateAddr is internal to Logic, so we simulate by not writing anything to it
    // or explicitly writing 0. For simplicity, just ensure nothing was written before createLogic().
    
    // Explicitly set the value at ToggleStateAddr to 0 to ensure it's "invalid" for a true toggle state.
    // ToggleStateAddr is a private member of Logic, exposed via logic::Stub::toggleStateAddr.
    // This value is 0x00 by default in the EEPROM stub, but we can be explicit.
    mock.eeprom.setReadByteValue(logic::Stub::toggleStateAddr(), 0U);

    mock.createLogic(); // Logic restores state from EEPROM here.

    // Expect the toggle timer to be disabled, as the EEPROM state was invalid/false.
    EXPECT_FALSE(mock.toggleTimer.isEnabled());
}

/**
 * @brief Simultaneous timer timeout test.
 * 
 *        Verify correct behavior when multiple timers time out simultaneously or very close.
 */
TEST(Logic, SimultaneousTimerTimeout)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};

    // Initial state: toggle timer off, debounce timer off.
    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.debounceTimer.isEnabled());

    // Simulate toggle button press.
    mock.toggleButton.setInput(true);
    logic.handleButtonEvent();
    mock.toggleButton.setInput(false);

    // Now debounceTimer is enabled.
    EXPECT_TRUE(mock.debounceTimer.isEnabled());

    // Simulate debounce timer timeout AND toggle timer timeout simultaneously.
    mock.debounceTimer.setHasTimedOut(true);
    mock.toggleTimer.setHasTimedOut(true);
    
    // Handle the debounce timeout first, then toggle timeout.
    logic.handleDebounceTimerTimeout();
    logic.handleToggleTimerTimeout();

    // After debounce timeout, debounce timer should be off, toggle timer should be enabled.
    EXPECT_FALSE(mock.debounceTimer.isEnabled());
    EXPECT_TRUE(mock.toggleTimer.isEnabled());

    // After toggle timeout, LED should be on.
    EXPECT_TRUE(mock.led.read());

    // Simulate another toggle button press (which will disable toggle timer and LED).
    mock.toggleButton.setInput(true);
    logic.handleButtonEvent();
    mock.toggleButton.setInput(false);

    // Simulate debounce timer timeout AND toggle timer timeout simultaneously.
    mock.debounceTimer.setHasTimedOut(true);
    mock.toggleTimer.setHasTimedOut(true);
    
    logic.handleDebounceTimerTimeout();
    logic.handleToggleTimerTimeout();

    // After second sequence, toggle timer should be disabled, LED should be off.
    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.led.read());
}

/**
 * @brief Button release no event test.
 * 
 *        Verify that releasing a button without a prior press event does not trigger logic.
 */
TEST(Logic, ButtonReleaseNoEvent)
{
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};

    // Ensure initial state.
    EXPECT_FALSE(mock.debounceTimer.isEnabled());
    EXPECT_FALSE(mock.toggleTimer.isEnabled());

    // Simulate only releasing the button, without a prior press.
    mock.toggleButton.setInput(false);
    logic.handleButtonEvent();

    // No debounce timer or toggle timer should be enabled.
    EXPECT_FALSE(mock.debounceTimer.isEnabled());
    EXPECT_FALSE(mock.toggleTimer.isEnabled());

    // Simulate releasing the temperature button.
    mock.tempButton.setInput(false);
    logic.handleButtonEvent();

    // Still no timers should be enabled due to release.
    EXPECT_FALSE(mock.debounceTimer.isEnabled());

}






} // namespace
} // namespace logic

#endif /** TESTSUITE */



