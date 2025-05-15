/**
 * @file quadtree.hpp
 * @author Radica
 * @brief 四叉树
 * @version 0.1
 * @date 2025-03-12
 *
 * @copyright Copyright (c) 2025 
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include <vector>

#ifndef DEBUG_ON
//#define DEBUG_ON
#endif

constexpr int CAPACITY = 4;    // 每个节点的最大容量
constexpr float G_EP = 1E-6f;  // 精度误差

template <typename T>
struct BBox
{
   public:
    T min_x, min_y, max_x, max_y;  // 包围盒

    BBox() = default;

    BBox(T x1, T y1, T x2, T y2) : min_x(x1), min_y(y1), max_x(x2), max_y(y2) {}

    BBox(const BBox<T>& box) : min_x(box.min_x), min_y(box.min_y), max_x(box.max_x), max_y(box.max_y) {}

    BBox<T>& operator=(const BBox<T>& box)
    {
        min_x = box.min_x;
        min_y = box.min_y;
        max_x = box.max_x;
        max_y = box.max_y;
        return *this;
    }

    template <typename U>
    BBox<U> cast() const
    {
        return BBox<U>(static_cast<U>(min_x), static_cast<U>(min_y), static_cast<U>(max_x), static_cast<U>(max_y));
    }

    bool contains(const BBox<T>& other) const
    {
        return (other.min_x + G_EP) >= min_x && (other.max_x - G_EP) <= max_x && (other.min_y + G_EP) >= min_y &&
               (other.max_y - G_EP) <= max_y;
    }

    bool intersects(const BBox<T>& other) const
    {
        return (other.min_x - G_EP) <= max_x && (other.max_x + G_EP) >= min_x && (other.min_y - G_EP) <= max_y &&
               (other.max_y + G_EP) >= min_y;
    }
};

// 包围盒回调函数，需要外部提供
template <typename ELEM_T, typename NUM_T = double>
using BBoxFunc = std::function<BBox<NUM_T>(const ELEM_T& elem)>;

/**
  * @brief 四叉树类
  * 
  * @tparam ELEM_T 存储的元素类型
  * @tparam NUM_T 包围盒的值类型
  */
template <typename ELEM_T, typename NUM_T = double>
class QuadTree
{
   private:
    std::vector<std::shared_ptr<QuadTree>> _children;  // 四个子节点 [左上，右上，左下，右下]
    std::vector<ELEM_T> _elem;                         // 存储的元素
    std::vector<ELEM_T> _spanElem;                     // 存储跨越多个子节点的元素
    BBoxFunc<ELEM_T, NUM_T> _getBBox;                  // 包围盒计算函数，用户提供
    BBox<NUM_T> _bbox;                                 // 节点包围盒 [min_x, min_y, max_x, max_y]

    /**
      * @brief 判断元素是否跨越多个子节点
      *
      * @param elem 元素
      * @param box 包围盒
      * @return bool 跨越返回true，不跨越返回false
      */
    bool crossQuadrant(const ELEM_T& elem, const BBox<NUM_T>& box)
    {
        // 计算中点，避免潜在溢出
        NUM_T mid_x = box.min_x + (box.max_x - box.min_x) / 2;
        NUM_T mid_y = box.min_y + (box.max_y - box.min_y) / 2;

        // 定义四个象限的边界框
        const std::array<BBox<NUM_T>, 4> quadrants = {{
            {box.min_x, box.min_y, mid_x, mid_y},  // 第一象限
            {mid_x, box.min_y, box.max_x, mid_y},  // 第二象限
            {box.min_x, mid_y, mid_x, box.max_y},  // 第三象限
            {mid_x, mid_y, box.max_x, box.max_y}   // 第四象限
        }};

        BBox<NUM_T> elem_bbox = _getBBox(elem);
        int count = 0;

        for (const auto& quad : quadrants)
        {
            if (quad.intersects(elem_bbox))
            {
                if (++count > 1)
                {  // 一旦超过一个象限相交，立即返回
                    return true;
                }
            }
        }

        return false;
    }

   public:
    QuadTree() = default;
    explicit QuadTree(const BBox<NUM_T>& box, BBoxFunc<ELEM_T, NUM_T> bboxFunc = nullptr)
        : _children(4, nullptr), _elem({}), _spanElem({}), _getBBox(bboxFunc), _bbox(box)
    {
    }

    QuadTree(const ELEM_T& elem, BBoxFunc<ELEM_T, NUM_T> bboxFunc)
        : _children(4, nullptr), _elem({}), _spanElem({}), _getBBox(bboxFunc), _bbox(_getBBox(elem))
    {
    }
    ~QuadTree() = default;

    /**
      * @brief 四叉树中插入元素
      *
      * @param elem 要插入的元素
      * @param box 元素的包围盒
      */
    int insert(ELEM_T elem)
    {
#ifdef DEBUG_ON
        // 打印当前节点的包围盒
        std::cout << "_bbox: " << _bbox.min_x << "," << _bbox.min_y << "," << _bbox.max_x << "," << _bbox.max_y
                  << std::endl;
        // 打印要插入元素的包围盒
        std::cout << "_getBBox(elem): " << _getBBox(elem).min_x << "," << _getBBox(elem).min_y << ","
                  << _getBBox(elem).max_x << "," << _getBBox(elem).max_y << std::endl;
#endif

        // 如果要插入的元素不在当前节点的包围盒内，则返回-1
        if (_bbox.contains(_getBBox(elem)) == false)
            return -1;

        // 如果当前节点有子节点，需要递归插入
        if (_children[0] != nullptr)
        {
            bool insertSuc = false;
            for (auto& child : _children)
            {
                int err = child->insert(elem);
                // 如果插入成功，则返回0
                if (err == 0)
                {
                    insertSuc = true;
                    break;
                }
            }
            // 如果插入失败，则返回0
            if (insertSuc == true)
                return 0;
        }
        // 如果当前节点的元素数量小于容量，且没有溢出元素，则将元素插入到当前节点
        if ((_elem.size() < CAPACITY) && (_spanElem.empty() == true))
            _elem.push_back(elem);
        // 如果当前节点的元素数量大于等于容量，且有溢出元素，则将元素插入到溢出元素中
        else
        {
            // 如果要插入的元素跨越了四个象限，则将元素插入到溢出元素中
            if (crossQuadrant(elem, _bbox) == true)
            {
                _spanElem.emplace_back(elem);
                return 0;
            }

            // 将当前节点划分为四个子节点
            subdivided();
            // 将当前节点的元素移动到子节点中
            std::vector<ELEM_T> elems = std::move(_elem);
            _elem.clear();
            // 将要插入的元素插入到子节点中
            elems.emplace_back(elem);
            for (auto& e : elems)
            {
                bool span = false;
                int tryCnt = 0;
                for (auto& child : _children)
                {
                    int err = child->insert(e);
                    // 如果插入成功，则跳出循环
                    if (err == 0)
                        break;
                    else
                        tryCnt++;
                }
                // 如果插入失败，则将元素插入到溢出元素中
                if (tryCnt == CAPACITY)
                    _spanElem.emplace_back(e);
            }
        }
        // 返回0
        return 0;
    }

    void remove(ELEM_T elem)
    {
        if (isLeaf() == true)
        {
            // 删除找到的元素
            _elem.erase(std::remove(_elem.begin(), _elem.end(), elem), _elem.end());
            return;
        }
        else
        {
            // 跨越数据移除
            _spanElem.erase(std::remove(_spanElem.begin(), _spanElem.end(), elem), _spanElem.end());
            // 递归删除子节点中的元素
            for (auto& child : _children)
                child->remove(elem);

            // 如果子节点为空，则将子节点置为空
            for (auto& child : _children)
            {
                if (child->isLeaf() == true)
                {
                    if (child->size() == 0)
                        child = nullptr;
                }
            }
        }
    }

    /**
      * @brief 四叉树中查询元素（粗查询）
      *
      * @param box 包围盒范围
      * @return std::vector<ELEM_T> 查找到的元素
      */
    std::vector<ELEM_T> query(const BBox<NUM_T>& range)
    {
        // 如果传入的范围和存储元素的包围盒有交集，则返回
        std::vector<ELEM_T> result;

        // 如果当前没有子节点，则搜索当前节点中存储的元素
        if (isLeaf() == true)
        {
            for (auto& e : _elem)
            {
                if (range.intersects(_getBBox(e)) == true)
                    result.emplace_back(e);
            }
            for (auto& e : _spanElem)
            {
                if (range.intersects(_getBBox(e)) == true)
                    result.emplace_back(e);
            }
        }
        else
        {
            // 如果当前节点有子节点，则递归查询子节点，递归之前需要检查跨越象限元素
            // 查询当前节点的跨越元素
            for (auto& e : _spanElem)
            {
                if (range.intersects(_getBBox(e)) == true)
                    result.emplace_back(e);
            }
            // 递归查询子节点
            for (auto& child : _children)
            {
                auto ret = child->query(range);
                result.insert(result.end(), ret.begin(), ret.end());
            }
        }

        return result;
    }

    /**
      * @brief 更新四叉树中元素
      *
      * @param elem 元素
      * @param box 包围盒
      */
    void update(ELEM_T elem, const std::array<NUM_T, 4>& box)
    {
        // 移除元素
        remove(elem);
        // 插入元素
        insert(elem, box);
    }

    /**
      * @brief 四叉树节点分裂并将元素插入到子节点中
      *
      */
    void subdivided()
    {
        // 计算当前节点的中心点
        NUM_T mid_x = (_bbox.min_x + _bbox.max_x) / 2.0;
        NUM_T mid_y = (_bbox.min_y + _bbox.max_y) / 2.0;

        // 创建四个子节点
        _children[0] = std::make_shared<QuadTree<ELEM_T, NUM_T>>(BBox<NUM_T>{_bbox.min_x, mid_y, mid_x, _bbox.max_y},
                                                                 _getBBox);  // 左上
        _children[1] = std::make_shared<QuadTree<ELEM_T, NUM_T>>(BBox<NUM_T>{mid_x, mid_y, _bbox.max_x, _bbox.max_y},
                                                                 _getBBox);  // 右上
        _children[2] = std::make_shared<QuadTree<ELEM_T, NUM_T>>(BBox<NUM_T>{_bbox.min_x, _bbox.min_y, mid_x, mid_y},
                                                                 _getBBox);  // 左下
        _children[3] = std::make_shared<QuadTree<ELEM_T, NUM_T>>(BBox<NUM_T>{mid_x, _bbox.min_y, _bbox.max_x, mid_y},
                                                                 _getBBox);  // 右下
    }

    /**
      * @brief 判断是否是叶子节点
      *
      * @return true 是叶子节点
      * @return false 不是叶子节点
      */
    bool isLeaf() const { return _children[0] == nullptr; }
};

#endif  // QUADTREE_H