/**
 * @brief GPIO driver STUB
 */
#pragma once

#include <cstdint>
#include "driver/gpio/interface.h"

namespace driver
{
namespace gpio
{

class Stub final : public Interface
{
public:
    Stub() noexcept
        : m_value{false}
        , m_initialized{true}
        , m_interruptEnabled{false}
    {}

    ~Stub() noexcept override = default;

    bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    Direction direction() const noexcept override
    {
        return Direction::Input;
    }

    bool read() const noexcept override
    {
        return m_value;
    }

    void write(bool output) noexcept override
    {
        if (m_initialized)
        {
            m_value = output;
        }
    }

    void toggle() noexcept override
    {
        if (m_initialized)
        {
            m_value = !m_value;
        }
    }

    void enableInterrupt(bool enable) noexcept override
    {
        if (m_initialized)
        {
            m_interruptEnabled = enable;
        }
    }

    void enableInterruptOnPort(bool enable) noexcept override
    {
        if (m_initialized)
        {
            m_interruptEnabled = enable;
        }
    }

    bool isInterruptEnabled() const noexcept
    {
        return m_interruptEnabled;
    }

    void setInitialized(bool initialized) noexcept
    {
        m_initialized = initialized;
        if (!m_initialized)
        {
            m_value = false;
            m_interruptEnabled = false;
        }
    }

private:
    bool m_value;
    bool m_initialized;
    bool m_interruptEnabled;
};

} // namespace gpio
} // namespace driver
