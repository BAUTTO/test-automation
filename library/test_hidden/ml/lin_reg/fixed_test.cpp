/**
 * @brief Unit tests for the fixed linear regression model.
 */
#include <gtest/gtest.h>

#include "ml/lin_reg/fixed.h"
#include "ml/types.h"

#ifdef TESTSUITE

namespace ml
{
namespace
{
/**
 * @brief Happy path test.
 * 
 *        Verify that the linear regression model predicts as intended during optimal conditions.
 */
TEST(LinRegFixed, HappyPath)
{
    // Create a linear regression model, expect the model to be untrained.
    lin_reg::Fixed linReg{};
    EXPECT_FALSE(linReg.isTrained());

    const Matrix1d trainIn{0.0, 1.0, 2.0, 3.0, 4.0};
    const Matrix2d trainOut{2.0, 4.0, 6.0, 8.0, 10.0};
    constexpr std::size_t epochCount{100U};
    constexpr double learningRate{0.01};

    // Verify that the model predicts 0 when untrained.
    for (const auto& input : trainIn)
    {
        constexpr double expectedPrediction{0.0};
        EXPECT_EQ(expectedPrediction, linReg.predict(input));
    }

    // Train the model, expect the training to succeed.
    EXPECT_TRUE(linReg.train(trainIn, trainOut, epochCount, learningRate));
    EXPECT_TRUE(linReg.isTrained());

    // Verify that the model predicts as intended.
    for (std::size_t i{}; i < trainIn.size(); ++i)
    {
        const double input{trainIn[i]};
        const double output{trainOut[i]};
        
        constexpr double precision{1e-6};
        EXPECT_NEAR(output, linReg.predict(input), precision);
    }
}

/**
 * @brief Linear regression training with invalid sets.
 * 
 *        Verify that the model doesn't get trained if no complete training sets are present.
 */
TEST(LinRegFixed, NoTrainingSets)
{
    // Case 1 - No training sets.
    {
        // Create a linear regression model, expect the model to be untrained.
        lin_reg::Fixed linReg{};
        EXPECT_FALSE(linReg.isTrained());

        const Matrix1d trainIn{};
        const Matrix2d trainOut{};
        constexpr std::size_t epochCount{100U};
        constexpr double learningRate{0.01};

        // Train the model, expect the training to fail.
        EXPECT_FALSE(linReg.train(trainIn, trainOut, epochCount, learningRate));
        EXPECT_FALSE(linReg.isTrained());
    }

    // Case 2 - No input sets.
    {
        // Create a linear regression model, expect the model to be untrained.
        lin_reg::Fixed linReg{};
        EXPECT_FALSE(linReg.isTrained());

        const Matrix1d trainIn{};
        const Matrix2d trainOut{2.0, 4.0, 6.0, 8.0, 10.0};
        constexpr std::size_t epochCount{100U};
        constexpr double learningRate{0.01};

        // Train the model, expect the training to fail.
        EXPECT_FALSE(linReg.train(trainIn, trainOut, epochCount, learningRate));
        EXPECT_FALSE(linReg.isTrained());
    }

    // Case 3 - No output sets.
    {
        // Create a linear regression model, expect the model to be untrained.
        lin_reg::Fixed linReg{};
        EXPECT_FALSE(linReg.isTrained());

        const Matrix1d trainIn{0.0, 1.0, 2.0, 3.0, 4.0};
        const Matrix2d trainOut{};
        constexpr std::size_t epochCount{100U};
        constexpr double learningRate{0.01};

        // Train the model, expect the training to fail.
        EXPECT_FALSE(linReg.train(trainIn, trainOut, epochCount, learningRate));
        EXPECT_FALSE(linReg.isTrained());
    }
}

/**
 * @brief Linear regression epoch count test.
 * 
 *        Verify that the model doesn't get trained if the epoch count is 0.
 */
TEST(LinRegFixed, EpochCount)
{
    // Case 1 - Epoch count is 0.
    {
        // Create a linear regression model, expect the model to be untrained.
        lin_reg::Fixed linReg{};
        EXPECT_FALSE(linReg.isTrained());

        const Matrix1d trainIn{0.0, 1.0, 2.0, 3.0, 4.0};
        const Matrix2d trainOut{2.0, 4.0, 6.0, 8.0, 10.0};
        constexpr std::size_t epochCount{0U};
        constexpr double learningRate{0.01};

        // Train the model, expect the training to fail.
        EXPECT_FALSE(linReg.train(trainIn, trainOut, epochCount, learningRate));
        EXPECT_FALSE(linReg.isTrained());
    }

    // Case 2 - Epoch count is greater than 0.
    {
        const Matrix1d trainIn{0.0, 1.0, 2.0, 3.0, 4.0};
        const Matrix2d trainOut{2.0, 4.0, 6.0, 8.0, 10.0};
        constexpr double learningRate{0.01};
        constexpr std::size_t epochMax{10000U};

        for (std::size_t epochCount{1U}; epochCount < epochMax; ++epochCount)
        {
            // Create a linear regression model, expect the model to be untrained.
            lin_reg::Fixed linReg{};
            EXPECT_FALSE(linReg.isTrained());

            // Train the model, expect the training to succeed.
            EXPECT_TRUE(linReg.train(trainIn, trainOut, epochCount, learningRate));   
            EXPECT_TRUE(linReg.isTrained());   
        }
    }
}

/**
 * @brief Linear regression learning rate test.
 * 
 *        Verify that the model doesn't get trained if the learning rate is invalid.
 */
TEST(LinRegFixed, LearningRate)
{
    const Matrix1d trainIn{0.0, 1.0, 2.0, 3.0, 4.0};
    const Matrix2d trainOut{2.0, 4.0, 6.0, 8.0, 10.0};

    constexpr std::size_t epochCount{100U};
    constexpr double lrMin{-10.0};
    constexpr double lrMax{10.0};
    constexpr double lrStep{0.1};

    for (double learningRate{lrMin}; learningRate <= lrMax; learningRate += lrStep)
    {
        // Create a linear regression model, expect the model to be untrained.
        lin_reg::Fixed linReg{};
        EXPECT_FALSE(linReg.isTrained());

        // Train the model, expect the training to fail unless the learning rate is withing the
        // the valid range.
        const bool valid{(0.0 < learningRate) && (1.0 >= learningRate)};
        EXPECT_EQ(valid, linReg.train(trainIn, trainOut, epochCount, learningRate));   
        EXPECT_EQ(valid, linReg.isTrained());   
    }
}
} // namespace
} // namespace ml

#endif /** TESTSUITE */