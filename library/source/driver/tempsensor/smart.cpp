#include "driver/tempsensor/smart.h"
#include "utils/utils.h"

namespace driver
{
namespace tempsensor
{

Smart::Smart(uint8_t channel,
             adc::Interface& adc,
             const ml::lin_reg::Fixed& model) noexcept
    : m_channel(channel)
    , m_adc(adc)
    , m_model(model)
    , m_initialized(false)
{
    if (m_adc.isInitialized() && m_adc.isChannelValid(m_channel) && m_model.isTrained())
    {
        m_initialized = true;
    }
}

bool Smart::isInitialized() const noexcept
{
    return m_initialized;
}

int16_t Smart::read() const noexcept
{
    if (!m_initialized)
    {
        return 0; // Or some error code, but return type is int16_t (temp). 0 is safe default.
    }
    
    double voltage = m_adc.inputVoltage(m_channel);
    double prediction = m_model.predict(voltage);
    
    return utils::round<int16_t>(prediction);
}

} // namespace tempsensor
} // namespace driver