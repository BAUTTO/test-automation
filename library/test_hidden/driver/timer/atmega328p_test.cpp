/**
 * @brief Unit tests for the ATmega328p timer driver.
 */
#include <stdint.h>
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

    timer.setTimeout_ms(0U);
    EXPECT_EQ(timer.timeout_ms(), 0U);

    timer.setTimeout_ms(60000U);
    EXPECT_EQ(timer.timeout_ms(), 60000U);
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Callback)
{
    resetCallbackFlag();

    timer::Atmega328p timer{10U, testCallback};
    timer.start();

    // Drive callback manually – do NOT wait on hasTimedOut()
    for (std::uint32_t i = 0; i < 1000U; ++i)
    {
        timer.handleCallback();
        if (callbackInvoked)
        {
            break;
        }
    }

    EXPECT_TRUE(callbackInvoked);
}

// -----------------------------------------------------------------------------
TEST(Timer_Atmega328p, Restart)
{
    resetCallbackFlag();

    timer::Atmega328p timer{10U, testCallback};
    timer.start();

    for (std::uint32_t i = 0; i < 5U; ++i)
    {
        timer.handleCallback();
    }

    timer.restart();

    EXPECT_TRUE(timer.isEnabled());

    for (std::uint32_t i = 0; i < 1000U; ++i)
    {
        timer.handleCallback();
        if (callbackInvoked)
        {
            break;
        }
    }

    EXPECT_TRUE(callbackInvoked);
}

} // namespace
} // namespace driver

#endif /** TESTSUITE */
