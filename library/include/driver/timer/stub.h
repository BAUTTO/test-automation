//! @todo Add timer driver stub here!
#pragma once

#include <stdint.h>   // OK i Microchip Studio / C-miljö
#include "driver/timer/interface.h"

namespace driver
{
namespace timer
{

class Stub final : public Interface
{
public:
    Stub() noexcept
        : m_initialized{true}
        , m_enabled{false}
        , m_timedOut{false}
        , m_timeoutMs{0}
    {}

    bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    bool isEnabled() const noexcept override
    {
        return m_enabled;
    }

    bool hasTimedOut() const noexcept override
    {
        return m_timedOut;
    }

    uint32_t timeout_ms() const noexcept override
    {
        return m_timeoutMs;
    }

    void setTimeout_ms(uint32_t timeout_ms) noexcept override
    {
        m_timeoutMs = timeout_ms;
    }

    void start() noexcept override
    {
        m_enabled  = true;
        m_timedOut = false;
    }

    void stop() noexcept override
    {
        m_enabled = false;
    }

    void toggle() noexcept override
    {
        m_enabled = !m_enabled;
    }

    void restart() noexcept override
    {
        m_enabled  = true;
        m_timedOut = false;
    }

    // ------------------------------------------------------------
    // TEST HELPER (not part of Interface)
    // Used by logic tests to simulate timer timeout
    void setTimedOut(bool timedOut) noexcept
    {
        m_timedOut = timedOut;
    }

private:
    // Highest-level state first
    bool m_initialized;

    // Runtime state
    bool m_enabled;
    bool m_timedOut;

    // Configuration
    uint32_t m_timeoutMs;
};

} // namespace timer
} // namespace driver
