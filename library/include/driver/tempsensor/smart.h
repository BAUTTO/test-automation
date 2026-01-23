//! @todo Add smart temperature sensor implementation here!

/**
 * @note Requirements:
 * 
 *       1. Prediction: The sensor shall predict temperature based on the input voltage from the 
 *                      associated pin.
 *       2. Voltage Measurement: The input voltage shall be measured using an A/D converter.
 *       3. Model Usage: The sensor shall use a pre-trained linear regression model for temperature 
 *                       prediction.
 *       4. Initialization Prerequisites:
 *           - The associated pin must be a valid ADC channel.
 *           - The ADC must be initialized.
 *           - The linear regression model must be pre-trained.
 */
#pragma once

#include <cstdint>

#include "driver/tempsensor/interface.h"
#include "driver/adc/interface.h"
#include "ml/lin_reg/fixed.h"

namespace driver
{
namespace tempsensor
{

/**
 * @brief Smart temperature sensor using ADC + linear regression.
 */
class Smart final : public Interface
{
public:
    /**
     * @brief Constructor.
     *
     * @param[in] adc      Reference to ADC driver.
     * @param[in] channel  ADC channel connected to the temperature sensor.
     * @param[in] model    Pre-trained linear regression model.
     */
    Smart(adc::Interface& adc,
          std::uint8_t channel,
          const ml::lin_reg::Fixed& model) noexcept;

    /**
     * @brief Check whether the sensor is initialized.
     */
    bool isInitialized() const noexcept override;

    /**
     * @brief Read temperature in degrees Celsius.
     */
    std::int16_t read() const noexcept override;

private:
    adc::Interface&            m_adc;
    std::uint8_t               m_channel;
    const ml::lin_reg::Fixed&  m_model;
    bool                       m_initialized;
};

} // namespace tempsensor
} // namespace driver
