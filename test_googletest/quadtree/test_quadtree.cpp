#include <gtest/gtest.h>

#include "Telos/quadtree/quadtree.hpp"

struct MyStruct
{
    int val;
    BBox<double> _box;
    MyStruct(int v, const BBox<double>& box) : val(v), _box(box){};

    BBoxFunc<MyStruct*, double> getBBox() const
    {
        return [](MyStruct* self) -> BBox<double>
        {
            return self->_box;
        };
    }
};

class QuadTreeTest : public ::testing::Test
{
   protected:
    QuadTree<MyStruct*, double> quadTree;
    void SetUp() override
    {
        MyStruct ori = {0, {-100, -100, 100, 100}};
        quadTree = {&ori, ori.getBBox()};
    }

    void TearDown() override {}
};

TEST_F(QuadTreeTest, insert)
{
    // 基础测试数据
    MyStruct p1 = {1, {-80, 40, -40, 60}};
    MyStruct p2 = {2, {-80, -80, -40, -20}};
    MyStruct p3 = {3, {40, -60, 60, -40}};
    MyStruct p4 = {4, {40, 30, 70, 60}};
    MyStruct p5 = {5, {-20, -20, 20, 20}};
    MyStruct p6 = {6, {10, 80, 20, 90}};
    MyStruct p7 = {7, {10, 55, 15, 65}};
    MyStruct p8 = {8, {35, 70, 40, 80}};

    MyStruct p9 = {9, {20, 70, 30, 80}};
    MyStruct p10 = {9, {20, 70, 30, 80}};
    MyStruct p11 = {9, {20, 70, 30, 80}};
    MyStruct p12 = {9, {20, 70, 30, 80}};
    MyStruct p13 = {9, {20, 70, 30, 80}};

    quadTree.insert(&p1);
    quadTree.insert(&p2);
    quadTree.insert(&p3);
    quadTree.insert(&p4);
    quadTree.insert(&p5);
    quadTree.insert(&p6);
    quadTree.insert(&p7);
    quadTree.insert(&p8);
    quadTree.insert(&p9);
    quadTree.insert(&p10);
    quadTree.insert(&p11);
    quadTree.insert(&p12);
    quadTree.insert(&p13);
}

TEST_F(QuadTreeTest, query)
{
    // 基础测试数据
    MyStruct p1 = {1, {-80, 40, -40, 60}};
    MyStruct p2 = {2, {-80, -80, -40, -20}};
    MyStruct p3 = {3, {40, -60, 60, -40}};
    MyStruct p4 = {4, {40, 30, 70, 60}};
    MyStruct p5 = {5, {-20, -20, 20, 20}};
    MyStruct p6 = {6, {10, 80, 20, 90}};
    MyStruct p7 = {7, {10, 55, 15, 65}};
    MyStruct p8 = {8, {35, 70, 40, 80}};
    MyStruct p9 = {9, {20, 70, 30, 80}};

    quadTree.insert(&p1);
    quadTree.insert(&p2);
    quadTree.insert(&p3);
    quadTree.insert(&p4);
    quadTree.insert(&p5);
    quadTree.insert(&p6);
    quadTree.insert(&p7);
    quadTree.insert(&p8);
    quadTree.insert(&p9);

    MyStruct range1 = {1, {-80, -80, 80, 80}};
    auto ret = quadTree.query(range1._box);
    EXPECT_EQ(ret.size(), 9);

    MyStruct range2 = {1, {0, 0, 100, 100}};
    auto ret2 = quadTree.query(range2._box);
    EXPECT_EQ(ret2.size(), 6);

    MyStruct range3 = {1, {0, 0, 0, 0}};
    auto ret3 = quadTree.query(range3._box);
    EXPECT_EQ(ret3.size(), 1);
}

TEST_F(QuadTreeTest, query2)
{
    // 基础测试数据
    MyStruct p1 = {1, {-80, 40, -40, 60}};
    MyStruct p2 = {2, {-80, -80, -40, -20}};
    MyStruct p3 = {3, {40, -60, 60, -40}};
    MyStruct p4 = {4, {40, 30, 70, 60}};
    MyStruct p5 = {5, {-20, -20, 20, 20}};
    MyStruct p6 = {6, {10, 80, 20, 90}};
    MyStruct p7 = {7, {10, 55, 15, 65}};
    MyStruct p8 = {8, {35, 70, 40, 80}};
    MyStruct p9 = {9, {20, 70, 30, 80}};

    MyStruct p10 = {9, {20, 70, 30, 80}};
    MyStruct p11 = {9, {20, 70, 30, 80}};
    MyStruct p12 = {9, {20, 70, 30, 80}};
    MyStruct p13 = {9, {20, 70, 30, 80}};

    quadTree.insert(&p1);
    quadTree.insert(&p2);
    quadTree.insert(&p3);
    quadTree.insert(&p4);
    quadTree.insert(&p5);
    quadTree.insert(&p6);
    quadTree.insert(&p7);
    quadTree.insert(&p8);
    quadTree.insert(&p9);
    quadTree.insert(&p10);
    quadTree.insert(&p11);
    quadTree.insert(&p12);
    quadTree.insert(&p13);

    MyStruct range1 = {1, {-80, -80, 80, 80}};
    auto ret = quadTree.query(range1._box);
    EXPECT_EQ(ret.size(), 13);

    MyStruct range2 = {1, {0, 0, 100, 100}};
    auto ret2 = quadTree.query(range2._box);
    EXPECT_EQ(ret2.size(), 10);

    MyStruct range3 = {1, {0, 0, 0, 0}};
    auto ret3 = quadTree.query(range3._box);
    EXPECT_EQ(ret3.size(), 1);
}