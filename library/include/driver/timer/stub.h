#pragma once

#include <stdint.h>
#include "driver/timer/interface.h"

namespace driver
{
namespace timer
{
class Stub final : public Interface
{
public:
    Stub() : m_isInitialized(true), m_isEnabled(false), m_hasTimedOut(false), m_timeout_ms(0), m_callback(nullptr), m_count(0) {}

    explicit Stub(uint32_t timeout_ms, void (*callback)() = nullptr, bool startTimer = false) noexcept
        : m_isInitialized(true), m_isEnabled(startTimer), m_hasTimedOut(false), m_timeout_ms(timeout_ms), m_callback(callback), m_count(0) {}

    bool isInitialized() const noexcept override { return m_isInitialized; }
    bool isEnabled() const noexcept override { return m_isEnabled; }
    bool hasTimedOut() const noexcept override { return m_hasTimedOut; }
    uint32_t timeout_ms() const noexcept override { return m_timeout_ms; }
    void setTimeout_ms(uint32_t timeout_ms) noexcept override { m_timeout_ms = timeout_ms; }
    void start() noexcept override { m_isEnabled = true; m_hasTimedOut = false; m_count = 0; }
    void stop() noexcept override { m_isEnabled = false; }
    void toggle() noexcept override { m_isEnabled = !m_isEnabled; }
    void restart() noexcept override { start(); }

    // Methods for testing
    void handleCallback() noexcept {
        if (m_isEnabled) {
            m_count++;
            if (m_count * 10 >= m_timeout_ms) { // Assuming 10ms per callback increment for simulation
                m_hasTimedOut = true;
                if (m_callback) {
                    m_callback();
                }
                m_count = 0; // Reset count after timeout
            }
        }
    }
    void setInitialized(bool initialized) { m_isInitialized = initialized; }
    void setEnabled(bool enabled) { m_isEnabled = enabled; }
    void setHasTimedOut(bool timedOut) { m_hasTimedOut = timedOut; }


private:
    bool m_isInitialized;
    bool m_isEnabled;
    bool m_hasTimedOut;
    uint32_t m_timeout_ms;
    void (*m_callback)();
    uint32_t m_count;
};
} // namespace timer
} // namespace driver