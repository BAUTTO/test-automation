/**
 * @brief Serial driver stub.
 */
#pragma once

#include <stdint.h>

#ifdef TESTSUITE
#include <iostream>
#endif

#include "container/vector.h"
#include "driver/serial/interface.h"

namespace driver
{
namespace serial
{
/**
 * @brief Serial driver stub.
 */
class Stub final : public Interface
{
public:
    explicit Stub(uint32_t baudRate = 9600) : m_baudRate(baudRate), m_isInitialized(true), m_isEnabled(false), m_txCount(0) {}

    uint32_t baudRate_bps() const noexcept override { return m_baudRate; }
    bool isInitialized() const noexcept override { return m_isInitialized; }
    bool isEnabled() const noexcept override { return m_isEnabled; }
    void setEnabled(bool enable) noexcept override { m_isEnabled = enable; }
    int16_t read(uint8_t* buffer, uint16_t size, uint16_t timeout_ms) const noexcept override { return -1; }

    // Methods for testing
    void setInitialized(bool initialized) { m_isInitialized = initialized; }
    uint32_t txCount() const { return m_txCount; }
    void resetTxCount() { m_txCount = 0; }


private:
    void print(const char* str) const noexcept override {
        (void) str; // Unused.
        m_txCount++;
    }

    uint32_t m_baudRate;
    bool m_isInitialized;
    bool m_isEnabled;
    mutable uint32_t m_txCount; // Use mutable because print is const
};

} // namespace serial
} // namespace driver
