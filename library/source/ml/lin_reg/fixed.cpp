/**
 * @brief Fixed linear regression implementation details.
 */
#include "ml/lin_reg/fixed.h"
#include "ml/types.h"

namespace ml
{
namespace lin_reg
{
namespace
{
// -----------------------------------------------------------------------------
constexpr size_t min(const size_t x, const size_t y) noexcept
{
    return x < y ? x : y;
}

// -----------------------------------------------------------------------------
constexpr bool isLearningRateValid(const double learningRate) noexcept
{
    return (0.0 < learningRate) && (1.0 >= learningRate);
}
} // namespace

// -----------------------------------------------------------------------------
Fixed::Fixed() noexcept
    : myWeight{}
    , myBias{}
    , myTrained{false}
{}

// -----------------------------------------------------------------------------
bool Fixed::isTrained() const noexcept { return myTrained; }

// -----------------------------------------------------------------------------
double Fixed::predict(const double input) const noexcept { return myWeight * input + myBias; }

// -----------------------------------------------------------------------------
bool Fixed::train(const Matrix1d& trainIn, const Matrix2d& trainOut, const size_t epochCount, 
                   const double learningRate) noexcept
{
    // Check the epoch count and learning rate, return false if invalid.
    if ((0U == epochCount) || !isLearningRateValid(learningRate)) { return false; }

    // Check the training set count, return false if invalid.
    const size_t setCount{min(trainIn.size(), trainOut.size())};
    if (0U == setCount) { return false; }

    // Clear the trainable parameters before starting training.
    myWeight = 0.0;
    myBias   = 0.0;

    // Train the model the specified number of epochs.
    for (size_t epoch{}; epoch < epochCount; ++epoch)
    {
        // Iterate through all training sets, skip randomization for simplicity.
        for (size_t i{}; i < setCount; ++i)
        {
            optimize(trainIn[i], trainOut[i], learningRate);
        }
    }
    // Return true to indicate success.
    myTrained = true;
    return myTrained;
}

// -----------------------------------------------------------------------------
void Fixed::optimize(const double input, const double output, const double learningRate) noexcept
{
    // Check the input, directly set bias to output if 0.0 (special case).
    // Otherwise, apply gradient descent to update both weight and bias.
    if (0.0 == input) { myBias = output; }
    else
    {
        const double error{output - predict(input)};
        myBias   += error * learningRate;
        myWeight += error * learningRate * input;
    }
}
} // namespace lin_reg
} // namespace ml