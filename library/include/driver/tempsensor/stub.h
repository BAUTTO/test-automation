#pragma once

#include <stdint.h>
#include "driver/tempsensor/interface.h"

namespace driver
{
namespace tempsensor
{
class Stub final : public Interface
{
public:
    explicit Stub(std::int16_t temp = 0) : m_temp(temp), m_initialized(true) {}

    bool isInitialized() const noexcept override { return m_initialized; }
    std::int16_t read() const noexcept override { return m_temp; }

    // Methods for testing
    void setInitialized(bool initialized) { m_initialized = initialized; }
    void setTemp(std::int16_t temp) { m_temp = temp; }

private:
    std::int16_t m_temp;
    bool m_initialized;
};
} // namespace tempsensor
} // namespace driver