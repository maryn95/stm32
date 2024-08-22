#include <gtest/gtest.h>
#include <Calculator.h>

TEST(CalculatorTest, TestExpressions)
{
    Calculator calc;
    ASSERT_EQ(calc.Add(6, 3), 9);
    ASSERT_EQ(calc.Subtract(6, 3), 3);
    ASSERT_EQ(calc.Multiply(6, 3), 18);
    ASSERT_EQ(calc.Divide(6, 3), 2);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}