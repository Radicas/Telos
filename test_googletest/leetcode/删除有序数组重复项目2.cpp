#include <gtest/gtest.h>

class ALGORITHM : public ::testing::Test
{
};

int removeDuplicates(int* nums, int numsSize)
{
    if (numsSize <= 2)
    {
        return numsSize;
    }
    int slow = 2, fast = 2;
    while (fast < numsSize)
    {
        if (nums[slow - 2] != nums[fast])
        {
            nums[slow] = nums[fast];
            ++slow;
        }
        ++fast;
    }
    return slow;
}
TEST_F(ALGORITHM, removeDuplicates)
{
    int nums[] = {1, 1, 1, 2, 2, 3};
    int new_size = removeDuplicates(nums, 6);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}

TEST_F(ALGORITHM, removeDuplicates2)
{
    int nums[] = {1, 1, 2, 2, 3};
    int new_size = removeDuplicates(nums, 5);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}
TEST_F(ALGORITHM, removeDuplicates3)
{
    int nums[] = {0, 0, 1, 1, 1, 1, 2, 3, 3};
    int new_size = removeDuplicates(nums, 9);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}
TEST_F(ALGORITHM, removeDuplicates4)
{
    int nums[] = {1};
    int new_size = removeDuplicates(nums, 1);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}

TEST_F(ALGORITHM, removeDuplicates5)
{
    int nums[] = {1, 1, 1};
    int new_size = removeDuplicates(nums, 3);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}
TEST_F(ALGORITHM, removeDuplicates6)
{
    int nums[] = {1, 1, 1, 1};
    int new_size = removeDuplicates(nums, 4);
    for (int i = 0; i < new_size; ++i)
    {
        printf("%d ", nums[i]);
    }
    printf("\nnew size: %d\n", new_size);
}