#ifndef COLLISION_SEARCH_H
#define COLLISION_SEARCH_H

#include <vector>

namespace Telos
{

class RXYTreeSearchItem
{
   private:
    void* mSearchAddr = nullptr;
    bool mIsAreaArray = false;
};

class BoundRect2D;
// RedEDA 搜索盒
class RXYTreeSearchBox
{
   private:
    std::vector<RXYTreeSearchItem> mSearchStack;
    int mCurSearchIndex = 0;             // 当前搜索到的索引
    BoundRect2D* mBoundRect = nullptr;  // 器件包围盒信息

   public:
    RXYTreeSearchBox();
    ~RXYTreeSearchBox();
};

}  // namespace Telos

#endif  // COLLISION_SEARCH_H