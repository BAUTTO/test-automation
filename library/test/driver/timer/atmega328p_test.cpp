/**
 * @brief Unit tests for the ATmega328p timer driver.
 */
#include <cstdint>
#include <gtest/gtest.h>

#include "arch/test/hw_platform.h"
#include "driver/timer/atmega328p.h"
#include "utils/utils.h"

#ifdef TESTSUITE

namespace driver
{
namespace
{
constexpr std::uint8_t MaxTimerCount{3U};

bool callbackInvoked{false};

void resetCallbackFlag() noexcept
{
    callbackInvoked = false;
}

void testCallback() noexcept
{
    callbackInvoked = true;
}

// -----------------------------------------------------------------------------
constexpr uint32_t getMaxCount(const uint32_t timeout_ms) noexcept
{
    constexpr double interruptInterval_ms{0.128};
	return utils::round<uint32_t>(timeout_ms / interruptInterval_ms);
}


// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Initialization)
{
    timer::Atmega328p timers[MaxTimerCount]{
        timer::Atmega328p{100U},
        timer::Atmega328p{200U},
        timer::Atmega328p{300U}
    };

    EXPECT_TRUE(timers[0].isInitialized());
    EXPECT_TRUE(timers[1].isInitialized());
    EXPECT_TRUE(timers[2].isInitialized());

    timer::Atmega328p extraTimer{500U};
    EXPECT_FALSE(extraTimer.isInitialized());
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, EnableDisable)
{
    timer::Atmega328p timer{100U};

    EXPECT_FALSE(timer.isEnabled());

    timer.start();
    EXPECT_TRUE(timer.isEnabled());

    timer.stop();
    EXPECT_FALSE(timer.isEnabled());

    timer.toggle();
    EXPECT_TRUE(timer.isEnabled());

    timer.toggle();
    EXPECT_FALSE(timer.isEnabled());
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Timeout)
{
    timer::Atmega328p timer{100U};

    EXPECT_EQ(timer.timeout_ms(), 100U);

    timer.setTimeout_ms(250U);
    EXPECT_EQ(timer.timeout_ms(), 250U);

    // Ensure that timeout cannot be set to 0. It should be unchanged, i.e. 250 ms,
    // since the request to set the timeout to 0 should be ignored.
    timer.setTimeout_ms(0U);
    EXPECT_EQ(timer.timeout_ms(), 250U);

    timer.setTimeout_ms(60000U);
    EXPECT_EQ(timer.timeout_ms(), 60000U);
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Callback)
{
    resetCallbackFlag();

    constexpr uint16_t timeout_ms{10U};

    // Compute how many times we need to invoke handleCallback to generate a timeout.
    constexpr uint32_t maxCount{getMaxCount(timeout_ms)};

    timer::Atmega328p timer{timeout_ms, testCallback};
    timer.start();

    // Drive callback manually – simulate 10 ms.
    // THis is generated via interrupts in the real hardware, we have to fake it here.
    for (std::uint32_t i = 0; i < maxCount; ++i)
    {
        timer.handleCallback();
    }

    EXPECT_TRUE(callbackInvoked);
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Restart)
{
    resetCallbackFlag();

    constexpr uint16_t timeout_ms{10U};

    // Compute how many times we need to invoke handleCallback to generate a timeout.
    constexpr uint32_t maxCount{getMaxCount(timeout_ms)};

    timer::Atmega328p timer{timeout_ms, testCallback};
    timer.start();

    // Simulate that the next timer interrupt will generate a timeout (9.99 out to 10 ms).
    for (std::uint32_t i = 0; i < maxCount - 1U; ++i)
    {
        timer.handleCallback();
    }

    // Restart the timer - we should need to start from scratch.
    timer.restart();

    EXPECT_TRUE(timer.isEnabled());

    // Simulate that the next timer interrupt will generate a timeout (9.99 out to 10 ms).
    for (std::uint32_t i = 0; i < maxCount - 1U; ++i)
    {
        timer.handleCallback();
    }

    // Verify that we don't get a timeout until after 10 ms, not 9.99 ms.
    EXPECT_FALSE(callbackInvoked);

    // Generate one more interrupt, now 10 ms have passed and we should have a timeout.
    timer.handleCallback();
    EXPECT_TRUE(callbackInvoked);
}

} // namespace
} // namespace driver

#endif /** TESTSUITE */
