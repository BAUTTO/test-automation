//! @todo Add temperature sensor stub here!
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
    Stub() noexcept
        : m_initialized{true}
        , m_temp{0}
    {}

    /**
     * @brief Check if the temperature sensor is initialized.
     */
    bool isInitialized() const noexcept override
    {
        return m_initialized;
    }

    std::int16_t read() const noexcept override
    {
        return m_temp;
    }
  
    void setTemperature(std::int16_t temp) noexcept
    {
        m_temp = temp;
    }


private:
    // Highest-level state first
    bool m_initialized;

    // Data second
    std::int16_t m_temp;

}; // class Stub

} // namespace tempsensor
} // namespace driver
