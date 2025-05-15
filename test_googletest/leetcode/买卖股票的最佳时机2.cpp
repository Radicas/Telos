#include <gtest/gtest.h>

class ALGORITHM : public ::testing::Test
{
};

/**
 * 对于leetcode中买卖股票的最佳时机2，你先不要给我答案。
 * 我说一下我的思路。首先，我看到这题时，第一感觉是要求解最优解，当然最优解可能有多个，但是我们只要返回最优值就行。
 * 然后我观察到，每次买入/卖出，都是线性的，从数组左边到右边。
 * 然后我就想，能否使用排列组合，列出所有用户可能的购买情况，然后算法最佳买卖法。
 * 对于这个思路，我感觉能求解出来，但是遇到一个困难？如何用代码描述所有买卖情况？
 * 我可能对排列组合的知识淡忘了。你有什么见解?
 */

int maxProfit(int* prices, int pricesSize) {
    return -1;
}

TEST_F(ALGORITHM, maxProfit)
{
    int prices[6] = {7, 1, 5, 3, 6, 4};
    ASSERT_EQ(7, maxProfit(prices, 6));
}

TEST_F(ALGORITHM, maxProfit2)
{
    int prices[5] = {1, 2, 3, 4, 5};
    ASSERT_EQ(4, maxProfit(prices, 5));
}

TEST_F(ALGORITHM, maxProfit3)
{
    int prices[5] = {7, 6, 4, 3, 1};
    ASSERT_EQ(0, maxProfit(prices, 5));
}

TEST_F(ALGORITHM, maxProfit4)
{
    int prices[2] = {1, 2};
}