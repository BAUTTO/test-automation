#pragma once

#include <stdint.h>
#include "driver/gpio/interface.h"

namespace driver
{
namespace gpio
{
class Stub final : public Interface
{
public:
    Stub() : m_isInitialized(true), m_direction(Direction::Input), m_input(false), m_output(false) {}

    explicit Stub(uint8_t pin, Direction direction, void (*callback)() = nullptr) noexcept
        : m_isInitialized(true), m_direction(direction), m_input(false), m_output(false) {}

    bool isInitialized() const noexcept override { return m_isInitialized; }
    Direction direction() const noexcept override { return m_direction; }
    bool read() const noexcept override { return (m_direction == Direction::Output) ? m_output : m_input; }
    void write(bool output) noexcept override { m_output = output; }
    void toggle() noexcept override { m_output = !m_output; }
    void enableInterrupt(bool enable) noexcept override {}
    void enableInterruptOnPort(bool enable) noexcept override {}
    bool isInterruptEnabled() const { return false; }

    // Methods for testing
    void setInitialized(bool initialized) { m_isInitialized = initialized; }
    void setInput(bool input) { m_input = input; }
    bool getOutput() const { return m_output; }

private:
    bool m_isInitialized;
    Direction m_direction;
    bool m_input;
    bool m_output;
};
} // namespace gpio
} // namespace driver