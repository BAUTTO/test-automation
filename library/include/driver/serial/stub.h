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
    /**
     * @brief Constructor.
     * 
     * @param[in] baudRate_bps The baud rate in bits per second (default = 9600 bps).
     */
    explicit Stub(const uint32_t baudRate_bps = 9600U) noexcept
        : myReadBuffer{}
        , myBaudRate_bps{baudRate_bps}
        , myEnabled{true}
        , m_txCount{0U}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /** 
     * @brief Get the baud rate of the serial device. 
     * 
     * @return The baud rate in bps (bits per second).
     */
    uint32_t baudRate_bps() const noexcept override
    {
        return myBaudRate_bps;
    }

    /**
     * @brief Check whether the serial device is initialized.
     * 
     * @return True if the device is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override
    {
        return true;
    }

    /**
     * @brief Check whether the serial device is enabled.
     * 
     * @return True if the serial device is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override
    {
        return myEnabled;
    }

    /**
     * @brief Set enablement of serial device.
     * 
     * @param[in] enable Indicate whether to enable the device.
     */
    void setEnabled(const bool enable) noexcept override
    {
        myEnabled = enable;
    }
    
    /**
     * @brief Read data from the serial port.
     * 
     * @param[out] buffer Read buffer.
     * @param[in] size Buffer size in bytes.
     * @param[in] timeout_ms Read timeout. Pass 0 to wait indefinitely.
     * 
     * @return The number of read characters, or -1 on error.
     */
    int16_t read(uint8_t* buffer,
                 const uint16_t size,
                 const uint16_t timeout_ms) const noexcept override
    {
        // Do not use the timeout in the stub implementation.
        (void)timeout_ms;

        if ((nullptr == buffer) || (size == 0U))
        {
            return -1;
        }

        const uint16_t storedBytes{
            static_cast<uint16_t>(myReadBuffer.size())
        };

        const uint16_t bytesToRead{
            size < storedBytes ? size : storedBytes
        };

        for (uint16_t i = 0U; i < bytesToRead; ++i)
        {
            buffer[i] = myReadBuffer[i];
        }

        return static_cast<int16_t>(bytesToRead);
    }

    /**
     * @brief Print the given string in the serial terminal.
     * 
     * @param[in] str The string to print.
     */
    void print(const char* str) const noexcept override
    {
        if ((!myEnabled) || (nullptr == str))
        {
            return;
        }

        // Count transmissions for logic tests
        ++m_txCount;

        #ifdef TESTSUITE
            std::cout << str;
        #endif
    }

    /**
     * @brief Clear the simulated read buffer.
     */
    void clearReadBuffer() noexcept
    {
        myReadBuffer.clear();
    } 

    /**
     * @brief Simulate received data by populating the read buffer.
     * 
     * @param[in] buffer Buffer containing the data to simulate.
     * @param[in] size Size of the buffer in bytes.
     */
    void setReadBuffer(const uint8_t* buffer,
                       const uint16_t size) noexcept
    { 
        if ((nullptr == buffer) || (size == 0U))
        {
            return;
        }

        myReadBuffer.resize(size);
        for (uint16_t i = 0U; i < size; ++i)
        {
            myReadBuffer[i] = buffer[i];
        }
    }

    /**
     * @brief Get number of transmitted messages (test helper).
     */
    std::size_t txCount() const noexcept
    {
        return m_txCount;
    }

    Stub(const Stub&)            = delete;
    Stub(Stub&&)                 = delete;
    Stub& operator=(const Stub&) = delete;
    Stub& operator=(Stub&&)      = delete;

private:
    /** Simulated read buffer. */
    container::Vector<uint8_t> myReadBuffer;

    /** Baud rate in bps (bits per second). */
    const uint32_t myBaudRate_bps;

    /** Indicate whether serial transmission is enabled. */
    bool myEnabled;

    /** Number of transmitted messages (test helper). */
    mutable std::size_t m_txCount;
};

} // namespace serial
} // namespace driver
