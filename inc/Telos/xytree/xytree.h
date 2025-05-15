#ifndef XYTREE_H
#define XYTREE_H

#include "Telos/macros.h"

#include <assert.h>
#include <vector>

namespace Telos
{

#define XY_THRESHOLD 16  // 树叶中area列表的最大长度(推荐4-25，默认16)

// XYTree子节点类型
enum XYTreeChildType
{
    XYTREE_CHILD_LEFT = 0,  // 左节点
    XYTREE_CHILD_MIDDLE,    // 中节点
    XYTREE_CHILD_RIGHT,     // 右节点
    XYTREE_CHILD_NUM,       // 表示数字3
    XYTREE_CHILD_INVALID    // 无效的子节点类型
};

// XYTree分割方向
enum XYTreeSplitDirection
{
    XYTREE_SPLIT_X = 0,  // X轴向分割
    XYTREE_SPLIT_Y,      // Y轴向分割
    XYTREE_SPLIT_NUM,
    XYTREE_SPLIT_INVALID  // 无效的分割方向
};

class BoundRect2D;

// 器件信息
class TELOS_PUBLIC ComponentArea
{
   private:
    void* mCompGeoData;       // 原始器件几何信息
    unsigned int mCompId;     // 原始器件的ID唯一标识
    BoundRect2D* mBoundRect;  // 器件的包围盒信息
    int mTypeId;              // ud1:数据类型
    void* mAddr;              // ud2:节点地址

   private:
    /**
     * @brief 设置用户数据
     * @param typeId
     * @param addr
     */
    void setUserData(int typeId, void* addr);

   public:
    ComponentArea();
    ~ComponentArea();

    /**
     * @brief 创建器件区域
     * @param minX
     * @param minY
     * @param maxX
     * @param maxY
     * @param typeId 器件类型
     * @param userDef 用户自定义行为
     * @return ComponentArea* 器件区域
     */
    static ComponentArea* createComponentArea(double minX, double minY, double maxX, double maxY, int typeId,
                                              void* userDef);

    /**
     * @brief 计算区域数组的总包围盒
     * @param areaArray 区域数组
     * @return BBox 总包围盒
     */
    static BoundRect2D calcAreaArrayBound(const std::vector<ComponentArea*>& areaArray);

    /**
     * @brief 打印器件信息
     * @param areaArray 器件区域数组
     * @param prefixInfo 前置信息
     */
    static void print(const std::vector<ComponentArea*>& areaArray, const char* prefixInfo);

    /**
     * @brief 设置器件的原始信息
     * @param compGeoData 器件的几何数据
     * @param compId 器件id
     */
    void setComponentOriginInfo(void* compGeoData, unsigned int compId);

    /**
     * @brief 获取器件ID
     * @return unsigned int 器件ID
     */
    unsigned int getCompId() const;

    /**
     * @brief 获取器件几何数据
     * @return void* 几何数据
     */
    void* getCompGeoData();

    /**
     * @brief 获取器件常量几何数据
     * @return const void* 几何数据
     */
    const void* getCompGeoData() const;

    /**
     * @brief 获取器件包围盒
     * @return BBox* 包围盒
     */
    BoundRect2D* getBoundRect();

    /**
     * @brief 获取器件常量包围盒
     * @return const BBox* 器件常量包围盒
     */
    const BoundRect2D* getBoundRect() const;

    /**
     * @brief 获取器件类型
     * @return int 器件类型
     */
    int getTypeId() const;

    /**
     * @brief 获取器件地址
     * @return void* 器件地址
     */
    void* getAddr();

    /**
     * @brief 获取常量器件地址
     * @return const void* 常量器件地址
     */
    const void* getAddr() const;

    /**
     * @brief 判断传入的器件区域是否和自己相同
     * @param otherArea 其他器件区域
     * @return true:相同 ｜ false:不相同
     */
    bool isEqual(const ComponentArea* otherArea) const;
};

class XYTreeLeaf;
/**
 * @brief XYTree树节点
 */
class XYTreeNode
{
   private:
    BoundRect2D* mBBox;                   // 树节点包围盒
    XYTreeNode* mParent;                  // 父节点
    void* mChild[XYTREE_CHILD_NUM];       // 左中右三个子节点
    bool mIsAreaArray[XYTREE_CHILD_NUM];  // 左中右三个子节点中，每个子节点是否为叶子节点
    XYTreeSplitDirection mSplitDir;       // 是否按x轴向分割(默认x轴向分割)
    double mSplitPos;                     // 分割点位置

   private:
    void isValid() const;

    // 查找子树中相交的Area区域
    void searchChild(XYTreeChildType childIndex, const BoundRect2D& srcRect,
                     std::vector<ComponentArea*>& resultArray) const;

    // 打印子树
    void printChild(const char* pSpan, const char* pChildStr, XYTreeChildType childIndex, int nLevel);

   public:
    XYTreeNode();
    ~XYTreeNode();

    // 创建一个树节点
    static XYTreeNode* createTreeNode(double aSplitPos, XYTreeSplitDirection aSplitDir = XYTREE_SPLIT_X);

    // 递归释放子节点，并根据需求释放树根自身
    static void freeNodesWithoutArea(XYTreeNode** aNode, bool bFreeRoot);

    // 返回在树中搜索一颗n层树的大致时间log(n):返回值>=1，即使树的层树为0
    static int getLogTime(int n);

    // 重新平衡化XYTree(中的树叶)，按照整个包围盒中的中点划分左右子树，以便保持较高的搜索效率（不由使用者直接调用）
    static void* rebalance(const XYTreeLeaf* aLeaf, bool& bOriginArray);

    void TreeAreaToArray(std::vector<ComponentArea*>& dstAreaArray) const;

    // 根据子节点的尺寸（假定每个子节点的包围盒已正确），重新计算当前节点的包围盒尺寸，并返回是否需要调整的标记
    bool adjustBoundBox();

    // 拓展当前节点包围盒到树叶，并返回最终的子树类型
    XYTreeNode* expandBoundToLeaf(const BoundRect2D* srcBoundRect, XYTreeChildType& childType);

    // 获取指定包围盒所属的树叶，并返回最终的子树类型
    XYTreeLeaf* getLeafWithBound(const BoundRect2D* srcBoundRect, XYTreeChildType& childType) const;

    // 判别 aSrcBound 属于哪个子树：aSrcBound在当前分割点的哪一侧
    XYTreeChildType getChildType(const BoundRect2D* aSrcBound) const;

    // 向指定的子树area列表中添加一个器件area
    const BoundRect2D* addLeafArea(XYTreeChildType aChildType, ComponentArea* area);

    bool deleteArea(const ComponentArea* srcArea);

    bool isFindArea(const ComponentArea* srcArea) const;

    bool isChildAreaArray(XYTreeChildType aChildType) const
    {
        assert(aChildType < XYTREE_CHILD_NUM);
        return mIsAreaArray[aChildType];
    }

    XYTreeNode* getChildNode(XYTreeChildType aChildType) const
    {
        if (isChildAreaArray(aChildType))
            return nullptr;
        assert(this == ((XYTreeNode*)(mChild[aChildType]))->mParent);
        return (XYTreeNode*)(mChild[aChildType]);
    }

    XYTreeNode* getParent() { return mParent; }
    const XYTreeNode* getParent() const { return mParent; }

    BoundRect2D* getBoundRect() { return mBBox; }
    const BoundRect2D* getBoundRect() const { return mBBox; }

    void search(const BoundRect2D& srcRect, std::vector<ComponentArea*>& resultArray) const;
    void print(const char* pSpan, int nLevel);
};

//RedEDA XYTree 树叶
class XYTreeLeaf
{
   private:
    BoundRect2D* mBoundRect;  //树节点的包围盒信息
    XYTreeNode* mParent;      //父节点
    std::vector<ComponentArea*> mAreaArray;

   public:
    XYTreeLeaf(XYTreeNode* aParent);
    ~XYTreeLeaf();

    const BoundRect2D* addArea(ComponentArea* area);
    void deleteArea(const ComponentArea* srcArea);
    BoundRect2D* adjustBoundBox();  //重新完整计算包围盒尺寸
    void expandBoundToLeaf(const BoundRect2D* srcBoundRect);
    void removeAreaArray(bool bDelete);
    void TreeAreaToArray(std::vector<ComponentArea*>& dstAreaArray, bool bRemove);

    void getJointArea(const BoundRect2D& srcRect, std::vector<ComponentArea*>& resultArray) const;
    std::vector<ComponentArea*>& getAreaArray() { return mAreaArray; }
    const std::vector<ComponentArea*>& getAreaArray() const { return mAreaArray; }

    XYTreeNode* getParent() { return mParent; }
    const XYTreeNode* getParent() const { return mParent; }

    BoundRect2D* getBoundRect() { return mBoundRect; }
    const BoundRect2D* getBoundRect() const { return mBoundRect; }

    void print(const char* pszPrefix) const;

};  //end of class RXYTreeLeaf

//RedEDA XYTree: 存放树根等相关信息
class TELOS_PUBLIC RXYTree
{

   private:
    XYTreeNode* mRootNode;  //树根节点
                            //void* mMemPool = nullptr; //内存池: 保留, 暂不用

   private:
    bool addAreaToTree(ComponentArea* area);

   public:
    RXYTree();
    ~RXYTree();

    void createTree(double aSplitPos, XYTreeSplitDirection aSplitDir = XYTREE_SPLIT_X);  //创建一颗XYTree
    bool addComponentArea(double aMinX, double aMinY, double aMaxX, double aMaxY, int aTypeId, void* aAddr);
    bool rebalance();  //重新平衡化整棵树
    void print();
    std::vector<ComponentArea*> getCollideAreaArray(double aMinX, double aMinY, double aMaxX,
                                                    double aMaxY);  //返回和指定矩形区碰撞的器件区域列表

};  //end of class REDALGO_CPP_PUBLIC RXYTree

}  // namespace Telos

#endif  // XYTREE_H