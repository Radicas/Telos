#include "Telos/xytree/xytree.h"
#include "Telos/xytree/bound_rect2d.h"

#include <stdio.h>
#include <string>

namespace Telos
{
void ComponentArea::setUserData(int typeId, void* addr)
{
    mTypeId = typeId;
    mAddr = addr;
}
ComponentArea::ComponentArea() : mCompGeoData(nullptr), mCompId(0), mBoundRect(new BoundRect2D()), mTypeId(-1), mAddr(nullptr)
{
    assert(mBoundRect);
}
ComponentArea::~ComponentArea()
{
    if (mBoundRect)
    {
        delete mBoundRect;
        mBoundRect = nullptr;
    }
}
ComponentArea* ComponentArea::createComponentArea(double minX, double minY, double maxX, double maxY, int typeId,
                                                  void* userDef)
{
    ComponentArea* area = new ComponentArea();
    assert(area);
    area->mBoundRect->setBound(minX, minY, maxX, maxY);
    area->setUserData(typeId, userDef);
    return area;
}
BoundRect2D ComponentArea::calcAreaArrayBound(const std::vector<ComponentArea*>& areaArray)
{
    assert(areaArray.size() > 0);
    BoundRect2D bounRect;
    for (const ComponentArea* area : areaArray)
    {
        bounRect.expandBound(area->getBoundRect());
    }
    return bounRect;
}
void ComponentArea::print(const std::vector<ComponentArea*>& areaArray, const char* prefixInfo)
{
    for (auto* area : areaArray)
    {
        if (nullptr == area)
        {
            int a = 0;
            ++a;
        }
        assert(area);
        const BoundRect2D* bound = area->getBoundRect();
        long long nAddr = (long long)(area->getAddr());

        // printf("%s: %f, %f, %f, %f, addr %ll\n", pPreFixInfo, bound->getMinX(), bound->getMinY(), bound->getMaxX(),
        //        bound->getMaxY(), area->getTypeId() < 1 ? "Leaf" : "Node", nAddr);
    }
}
void ComponentArea::setComponentOriginInfo(void* compGeoData, unsigned int compId)
{
    mCompGeoData = compGeoData;
    mCompId = compId;
}

unsigned int ComponentArea::getCompId() const
{
    return mCompId;
}
void* ComponentArea::getCompGeoData()
{
    return mCompGeoData;
}
const void* ComponentArea::getCompGeoData() const
{
    return mCompGeoData;
}
BoundRect2D* ComponentArea::getBoundRect()
{
    return mBoundRect;
}
const BoundRect2D* ComponentArea::getBoundRect() const
{
    return mBoundRect;
}
int ComponentArea::getTypeId() const
{
    return mTypeId;
}
void* ComponentArea::getAddr()
{
    return mAddr;
}
const void* ComponentArea::getAddr() const
{
    return mAddr;
}

bool ComponentArea::isEqual(const ComponentArea* otherArea) const
{
    return mBoundRect->isEqual(otherArea->getBoundRect()) && mTypeId == otherArea->mTypeId && mAddr == otherArea->mAddr;
}

void XYTreeNode::isValid() const
{
    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)
    {
        if (!mIsAreaArray[i])
        {
            assert(mChild[i]);  // 若子节点不是树叶area，则一定为有效的子节点
        }
    }
}
void XYTreeNode::searchChild(XYTreeChildType childIndex, const BoundRect2D& srcRect,
                              std::vector<ComponentArea*>& resultArray) const
{
    if (nullptr == mChild[childIndex])
    {
        //assert(false);
        return;
    }

    if (mIsAreaArray[childIndex])  //左子树为树叶
    {
        const XYTreeLeaf* leaf = (const XYTreeLeaf*)mChild[childIndex];
        leaf->getJointArea(srcRect, resultArray);
    }
    else
    {
        XYTreeNode* node = (XYTreeNode*)mChild[childIndex];
        assert(node);
        node->search(srcRect, resultArray);  //递归：继续向下遍历左子树
    }
}
void XYTreeNode::printChild(const char* pSpan, const char* pChildStr, XYTreeChildType childIndex, int nLevel)
{
    if (mChild[childIndex])
    {
        const BoundRect2D* childRect = nullptr;
        if (!mIsAreaArray[childIndex])
        {
            const XYTreeNode* node = (XYTreeNode*)mChild[childIndex];
            childRect = node->getBoundRect();
            assert(childRect);
            printf("%s child %s: (%f, %f, %f, %f) \n", pSpan, pChildStr, childRect->getMinX(), childRect->getMinY(),
                   childRect->getMaxX(), childRect->getMaxY());
        }
        else
        {
            //printf("%s child %s: \n", pSpan, pChildStr);
            const XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[childIndex];
            childRect = leaf->getBoundRect();
            printf("%s Leaf %s: (%f, %f, %f, %f) \n", pSpan, pChildStr, childRect->getMinX(), childRect->getMinY(),
                   childRect->getMaxX(), childRect->getMaxY());
        }

        if (mIsAreaArray[childIndex])
        {
            const XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[childIndex];
            leaf->print((pSpan + std::string("  child")).c_str());
        }
        else
        {
            XYTreeNode* node = (XYTreeNode*)mChild[childIndex];
            node->print((pSpan + std::string("   ")).c_str(), nLevel + 1);
        }
    }
    else
    {
        assert(mIsAreaArray[childIndex]);
    }
}
XYTreeNode::XYTreeNode() : mBBox(new BoundRect2D()), mParent(nullptr), mSplitDir(XYTREE_SPLIT_X), mSplitPos(-1)
{
    assert(mBBox);
    for (auto& child : mChild)
    {
        child = nullptr;
    }

    for (auto& isArea : mIsAreaArray)
    {
        isArea = true;
    }

    isValid();
}
XYTreeNode::~XYTreeNode()
{
    if (mBBox)
    {
        delete mBBox;
        mBBox = nullptr;
    }

    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)
    {
        if (mChild[i])
        {
            if (mIsAreaArray[i])  // If the current child is a leaf node
            {
                XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[i];
                delete leaf;
                leaf = nullptr;
            }
            else
            {
                XYTreeNode* node = (XYTreeNode*)mChild[i];
                delete node;
                node = nullptr;
            }
            mChild[i] = nullptr;
        }
        else
        {
            assert(mIsAreaArray[i]);
        }
    }
}
XYTreeNode* XYTreeNode::createTreeNode(double aSplitPos, XYTreeSplitDirection aSplitDir /* = XYTREE_SPLIT_X*/)
{
    XYTreeNode* node = new XYTreeNode();
    assert(node);
    node->mSplitPos = aSplitPos;
    node->mSplitDir = aSplitDir;
    return node;
}
void XYTreeNode::freeNodesWithoutArea(XYTreeNode** aNode, bool bFreeRoot)
{
    assert(aNode && *aNode);
    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)  // 遍历左中右3个子树，释放子节点（但不释放树叶）
    {
        if ((*aNode)->mChild[i])
        {
            if (!(*aNode)->mIsAreaArray[i])
            {
                XYTreeNode* node = (XYTreeNode*)((*aNode)->mChild[i]);
                freeNodesWithoutArea(&node, true);
            }
            else
            {
                XYTreeLeaf* leaf = (XYTreeLeaf*)((*aNode)->mChild[i]);

                delete leaf;
            }
            (*aNode)->mChild[i] = nullptr;
        }
    }

    if (bFreeRoot)  // 释放子节点自身
    {
        delete (*aNode);
        *aNode = nullptr;
    }
}
int XYTreeNode::getLogTime(int n)
{
    int i = 0;
    do
    {
        ++i;
        n >>= 1;
    } while (n > 0);
    return i;
}
void* XYTreeNode::rebalance(const XYTreeLeaf* aLeaf, bool& bOriginArray)
{
    if (nullptr == aLeaf)
    {
        bOriginArray = true;
        return (void*)aLeaf;
    }
    const std::vector<ComponentArea*>* areaArray = &aLeaf->getAreaArray();
    int areaNum = areaArray->size();
    if (areaNum < XY_THRESHOLD)
    {
        bOriginArray = true;
        return (void*)aLeaf;
    }
    assert(areaNum < 10);

    BoundRect2D resultRect = ComponentArea::calcAreaArrayBound(*areaArray);
    double midX = (resultRect.getMinX() + resultRect.getMaxX()) / 2.0;
    double midY = (resultRect.getMinY() + resultRect.getMaxY()) / 2.0;

    int nLeftX = 0, nRightX = 0, nLeftY = 0, nRightY = 0;
    for (const ComponentArea* area : *areaArray)
    {
        const BoundRect2D* rect = area->getBoundRect();
        if (rect->getMaxX() < midX)
            ++nLeftX;
        if (rect->getMinX() > midX)
            ++nRightX;
        if (rect->getMaxY() < midY)
            ++nLeftY;
        if (rect->getMinY() > midY)
            ++nRightY;
    }
    bool bSplitDirX = true;
    double fom_x = 0.0, fom_y = 0.0;
    if (nLeftX && nRightX && nLeftY && nRightY)
    {
        fom_x = (double)nLeftX * (double)XYTreeNode::getLogTime(nLeftX) +
                (double)areaNum * (double)XYTreeNode::getLogTime(areaNum - nLeftX - nRightX) +
                (double)nRightX * (double)XYTreeNode::getLogTime(nRightX);  //X方向最优值
        fom_y = (double)nLeftY * (double)XYTreeNode::getLogTime(nLeftY) +
                (double)areaNum * (double)XYTreeNode::getLogTime(areaNum - nLeftY - nRightY) +
                (double)nRightY * (double)XYTreeNode::getLogTime(nRightY);  //Y方向最优值
        bSplitDirX = (fom_x <= fom_y);                                       //沿着最优值更小的方向分割
    }
    else if (nLeftX && nRightX)
    {
        bSplitDirX = true;  //X方向分割
    }
    else
    {
        bSplitDirX = false;  //Y方向分割
    }

    XYTreeNode* tree = nullptr;
    if (bSplitDirX)
    {
        if (nLeftX + nRightX < XY_THRESHOLD)  //若左右两侧的尺寸小于门限，
        {
            bOriginArray = true;
            return (void*)aLeaf;  //返回树叶area链表头指针11，无需继续平衡化
        }

        tree = XYTreeNode::createTreeNode(
            midX, XYTREE_SPLIT_X);  //创建一个子节点（近邻树叶）：坐标初值xmid，默认X方向垂直分割，且存放树叶
        for (
            ComponentArea* area :
            *areaArray)  //从当前树叶area列表头开始，遍历每个area，将完全在xmid左侧的area加入到左子树页头，完全在xmid右侧的area加入右子树页头，其余的加入中子树页头
        {
            assert(area);
            if (area->getBoundRect()->getMaxX() < midX)  //若当前area在中点左侧，则将当前area加入到新节点的左子树叶头
            {
                tree->addLeafArea(XYTREE_CHILD_LEFT, area);
            }
            else if (area->getBoundRect()->getMinX() >
                     midX)  //若当前area在中点右侧，则将当前area加入到新节点的右子树叶头
            {
                tree->addLeafArea(XYTREE_CHILD_RIGHT, area);
            }
            else
            {
                tree->addLeafArea(XYTREE_CHILD_MIDDLE, area);  //否则，则将当前area加入到新节点的中子树叶头
            }
        }
    }
    else
    {
        if (!nLeftY || !nRightY || nLeftY + nRightY < XY_THRESHOLD)  //若左右两侧的尺寸小于门限，或所有area分布在单侧
        {
            bOriginArray = true;
            return (void*)aLeaf;  //返回树叶area链表头指针11，无需继续平衡化
        }

        tree = XYTreeNode::createTreeNode(midY, XYTREE_SPLIT_Y);  //创建一个子节点（近邻树叶）：Y方向分割，坐标改为yMid
        for (
            ComponentArea* area :
            *areaArray)  //从当前树叶area列表头开始，遍历每个area，将完全在yMid左侧的area加入到左子树页头，完全在yMid右侧的area加入右子树页头，其余的加入中子树页头
        {
            assert(area);
            if (area->getBoundRect()->getMaxY() < midY)  //若当前area在中点左侧，则将当前area加入到新节点的左子树叶头
            {
                tree->addLeafArea(XYTREE_CHILD_LEFT, area);
            }
            else if (area->getBoundRect()->getMinY() >
                     midY)  //若当前area在中点右侧，则将当前area加入到新节点的右子树叶头
            {
                tree->addLeafArea(XYTREE_CHILD_RIGHT, area);
            }
            else
            {
                tree->addLeafArea(XYTREE_CHILD_MIDDLE, area);  //否则，则将当前area加入到新节点的中子树叶头
            }
        }
    }
    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)
    {
        assert(tree->mIsAreaArray[i] && tree->mChild[i]);
        XYTreeLeaf* oldLeaf = (XYTreeLeaf*)tree->mChild[i];

        void* pAddr = rebalance(oldLeaf, tree->mIsAreaArray[i]);
        if (tree->mIsAreaArray[i])
        {
            assert(pAddr == oldLeaf && tree->mChild[i] == pAddr);
        }
        else
        {
            assert(pAddr != oldLeaf && tree->mChild[i] == pAddr);
            oldLeaf->removeAreaArray(false);
            delete oldLeaf;

            tree->mChild[i] = pAddr;
        }
    }
    bOriginArray = false;
    return tree;
}
void XYTreeNode::TreeAreaToArray(std::vector<ComponentArea*>& dstAreaArray) const
{
    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)
    {
        if (mChild[i])
        {
            if (mIsAreaArray[i])  //若当前子树是树叶area列表
            {
                XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[i];
                leaf->TreeAreaToArray(dstAreaArray, true);
            }
            else
            {
                XYTreeNode* node = (XYTreeNode*)mChild[i];
                node->TreeAreaToArray(dstAreaArray);
            }
        }
        else
        {
            assert(mIsAreaArray[i]);
        }
    }
}
bool XYTreeNode::adjustBoundBox()
{
    BoundRect2D resultRect;
    for (int i = XYTREE_CHILD_LEFT; i < XYTREE_CHILD_NUM; ++i)
    {
        if (mChild[i])
        {
            if (mIsAreaArray[i])  //若当前子树是树叶area列表
            {
                XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[i];
                resultRect.expandBound(leaf->getBoundRect() /*leaf->adjustBoundBox()*/);
            }
            else  //中间层节点
            {
                XYTreeNode* node = (XYTreeNode*)mChild[i];
                //node->adjustBoundBox(); //递归调整包围盒
                resultRect.expandBound(node->getBoundRect());
            }
        }
        else
        {
            assert(mIsAreaArray[i]);
        }
    }

    bool bEqual = mBBox->isEqual(&resultRect);
    if (bEqual)
    {
        mBBox->setBound(&resultRect);
    }
    return !bEqual;
}
XYTreeNode* XYTreeNode::expandBoundToLeaf(const BoundRect2D* srcBoundRect, XYTreeChildType& childType)
{
    assert(nullptr == mParent);    //当前节点为根节点
    XYTreeNode* curNode = this;   // rootNode;
    childType = XYTREE_CHILD_NUM;  //子节点类型
    do
    {
        BoundRect2D* treeNodeBound = curNode->getBoundRect();
        treeNodeBound->expandBound(srcBoundRect);
        childType = curNode->getChildType(srcBoundRect);
        assert(childType < XYTREE_CHILD_NUM);
        if (curNode->isChildAreaArray(childType))
        {
            XYTreeLeaf* leaf = (XYTreeLeaf*)curNode->mChild[childType];
            if (leaf)
            {
                leaf->expandBoundToLeaf(srcBoundRect);
            }
            break;
        }
        curNode = curNode->getChildNode(childType);
        assert(curNode && curNode->mParent);
    } while (true);
    return curNode;
}
XYTreeLeaf* XYTreeNode::getLeafWithBound(const BoundRect2D* srcBoundRect, XYTreeChildType& childType) const
{
    assert(nullptr == mParent);         //当前节点为根节点
    const XYTreeNode* curNode = this;  // rootNode;
    childType = XYTREE_CHILD_NUM;       //子节点类型
    do
    {
        childType = curNode->getChildType(srcBoundRect);
        assert(childType < XYTREE_CHILD_NUM);
        if (curNode->isChildAreaArray(childType))
        {
            break;
        }
        curNode = curNode->getChildNode(childType);
        assert(curNode && curNode->mParent);
    } while (true);
    return curNode ? (XYTreeLeaf*)(curNode->mChild[childType]) : nullptr;
}
XYTreeChildType XYTreeNode::getChildType(const BoundRect2D* aSrcBound) const
{
    assert(aSrcBound && aSrcBound->isValid());
    assert(mBBox && mBBox->isValid());
    switch (mSplitDir)
    {
        case XYTREE_SPLIT_X:  //X轴向分割
            if (aSrcBound->getMaxX() < mSplitPos)
                return XYTREE_CHILD_LEFT;
            else if (aSrcBound->getMinX() > mSplitPos)
                return XYTREE_CHILD_RIGHT;
            else
                return XYTREE_CHILD_MIDDLE;
            break;
        case XYTREE_SPLIT_Y:  //Y轴向分割
            if (aSrcBound->getMaxY() < mSplitPos)
                return XYTREE_CHILD_LEFT;
            else if (aSrcBound->getMinY() > mSplitPos)
                return XYTREE_CHILD_RIGHT;
            else
                return XYTREE_CHILD_MIDDLE;
            break;
        default:
            assert(false);
    }
    return XYTREE_CHILD_INVALID;
}
const BoundRect2D* XYTreeNode::addLeafArea(XYTreeChildType aChildType, ComponentArea* area)
{
    assert(mIsAreaArray[aChildType]);
    if (nullptr == mChild[aChildType])
    {
        mChild[aChildType] = new XYTreeLeaf(this);
    }
    XYTreeLeaf* leaf = (XYTreeLeaf*)mChild[aChildType];
    mBBox->expandBound(leaf->addArea(area));
    return mBBox;
}
bool XYTreeNode::deleteArea(const ComponentArea* srcArea)
{
    assert(srcArea && srcArea->getBoundRect()->isValid());
    XYTreeChildType childType = XYTREE_CHILD_NUM;
    XYTreeLeaf* curLeaf = getLeafWithBound(srcArea->getBoundRect(), childType);
    if (nullptr == curLeaf)
    {
        return false;
    }
    curLeaf->deleteArea(srcArea);
    XYTreeNode* node = curLeaf->getParent();
    while (node)
    {
        if (!node->adjustBoundBox())
        {
            return true;
        }
        node = node->mParent;
    }
    return true;
}
bool XYTreeNode::isFindArea(const ComponentArea* srcArea) const
{
    assert(srcArea && srcArea->getBoundRect()->isValid());
    XYTreeChildType childType = XYTREE_CHILD_NUM;
    const XYTreeLeaf* curLeaf = getLeafWithBound(srcArea->getBoundRect(), childType);
    if (nullptr == curLeaf)
    {
        return false;
    }

    const std::vector<ComponentArea*>* areaArray = &curLeaf->getAreaArray();
    assert(areaArray->size() > 0);
    for (int i = 0, nCount = areaArray->size(); i < nCount; ++i)  //遍历树叶中的area器件列表
    {
        ComponentArea* area = (*areaArray)[i];
        assert(area);
        if (area->isEqual(srcArea))
        {
            return true;
        }
    }
    return false;
}
void XYTreeNode::search(const BoundRect2D& srcRect, std::vector<ComponentArea*>& resultArray) const
{
    assert(mBBox);
    if (mBBox->isDisjoint(&srcRect))
    {
        return;
    }

    XYTreeChildType childType =
        getChildType(&srcRect);  //根据当前树节点的coord，以及给定的区域范围，判别当前区域属于左中右哪个子节点
    if (XYTREE_CHILD_LEFT == childType || XYTREE_CHILD_MIDDLE == childType)  //遍历左子树
    {
        searchChild(XYTREE_CHILD_LEFT, srcRect, resultArray);
    }

    //遍历中子树（注意：必须总是遍历中子树）
    searchChild(XYTREE_CHILD_MIDDLE, srcRect, resultArray);

    if (XYTREE_CHILD_MIDDLE == childType || XYTREE_CHILD_RIGHT == childType)  //遍历右子树
    {
        searchChild(XYTREE_CHILD_RIGHT, srcRect, resultArray);
    }
}
void XYTreeNode::print(const char* pSpan, int nLevel)
{
    // printf("%s Node level %d split cord: %f(%E), direction: %s, (%f, %f, %f) \n", pSpan, nLevel, mSplitPos, mSplitPos,
    //        XYTREE_SPLIT_X == mSplitDir ? "x" : "y", mBoundRect->getMinX(), mBoundRect->getMinY(), mBoundRect->getMaxX(),
    //        mBoundRect->getMaxY());
    printChild(pSpan, "Left", XYTREE_CHILD_LEFT, nLevel);
    printChild(pSpan, "Middle", XYTREE_CHILD_MIDDLE, nLevel);
    printChild(pSpan, "Right", XYTREE_CHILD_RIGHT, nLevel);
}

XYTreeLeaf::XYTreeLeaf(XYTreeNode* aParent) : mBoundRect(new BoundRect2D()), mParent(aParent), mAreaArray()
{
    assert(mBoundRect);
}
XYTreeLeaf::~XYTreeLeaf()
{
    if (mBoundRect)
    {
        delete mBoundRect;
        mBoundRect = nullptr;
    }
    removeAreaArray(true);
}
const BoundRect2D* XYTreeLeaf::addArea(ComponentArea* area)
{
    assert(mParent);
    mAreaArray.insert(mAreaArray.begin(), area);
    mBoundRect->expandBound(area->getBoundRect());
    return mBoundRect;
}
void XYTreeLeaf::deleteArea(const ComponentArea* srcArea)
{
    std::vector<ComponentArea*>* areaArray = &mAreaArray;
    assert(areaArray->size() > 0);
    for (int i = 0, nCount = areaArray->size(); i < nCount; ++i)
    {
        ComponentArea* area = (*areaArray)[i];
        assert(area);
        if (area->isEqual(srcArea))
        {
            delete area;  // 删除当前area
            areaArray->erase(areaArray->begin() + i);
            adjustBoundBox();
            break;
        }
    }
}
BoundRect2D* XYTreeLeaf::adjustBoundBox()
{
    BoundRect2D resultRect = ComponentArea::calcAreaArrayBound(mAreaArray);
    mBoundRect->setBound(&resultRect);
    return mBoundRect;
}
void XYTreeLeaf::expandBoundToLeaf(const BoundRect2D* srcBoundRect)
{
    mBoundRect->expandBound(srcBoundRect);
}
void XYTreeLeaf::removeAreaArray(bool bDelete)
{
    if (bDelete)
    {
        for (auto* area : mAreaArray)
        {
            delete area;
            area = nullptr;
        }
    }
    mAreaArray.clear();
}
void XYTreeLeaf::TreeAreaToArray(std::vector<ComponentArea*>& dstAreaArray, bool bRemove)
{
    const std::vector<ComponentArea*>* areaArray = &mAreaArray;
    assert(areaArray->size() > 0);
    for (auto* area : *areaArray)
    {
        assert(area);
        dstAreaArray.push_back(area);
    }
    if (bRemove)
    {
        mAreaArray.clear();
    }
}
void XYTreeLeaf::getJointArea(const BoundRect2D& srcRect, std::vector<ComponentArea*>& resultArray) const
{
    assert(mAreaArray.size() > 0);
    if (srcRect.isDisjoint(mBoundRect))  // 若包围盒不相交
    {
        return;
    }

    for (ComponentArea* area : mAreaArray)
    {
        assert(area);
        if (!srcRect.isDisjoint(area->getBoundRect()))  // 若包围盒相交
        {
            resultArray.push_back(area);
        }
    }
}
void XYTreeLeaf::print(const char* pszPrefix) const
{
    const std::vector<ComponentArea*>* areaArray = &mAreaArray;
    assert(areaArray->size() > 0);
    ComponentArea::print(*areaArray, pszPrefix);
}

RXYTree::RXYTree() : mRootNode(nullptr) {}
RXYTree::~RXYTree() {}
void RXYTree::createTree(double aSplitPos, XYTreeSplitDirection aSplitDir /*= XYTREE_SPLIT_X*/)
{
    if (nullptr == mRootNode)
    {
        mRootNode = XYTreeNode::createTreeNode(aSplitPos, aSplitDir);
    }
    assert(mRootNode);
}
bool RXYTree::addComponentArea(double aMinX, double aMinY, double aMaxX, double aMaxY, int aTypeId, void* aAddr)
{
    assert(mRootNode);
    if (nullptr == mRootNode)
    {
        return false;
    }
    ComponentArea* area = ComponentArea::createComponentArea(aMinX, aMinY, aMaxX, aMaxY, aTypeId, aAddr);
    return addAreaToTree(area);
}
bool RXYTree::rebalance()
{
    if (nullptr == mRootNode)
        return false;
    assert(nullptr == mRootNode->getParent());

    XYTreeLeaf leaf(nullptr);
    std::vector<ComponentArea*>& areaArray = leaf.getAreaArray();
    mRootNode->TreeAreaToArray(areaArray);
    printf("area num: %d \n", (int)areaArray.size());

#if 0
    RXYTreeNode::freeNodesWithoutArea(&mRootNode, true);
#else
    delete mRootNode;
    mRootNode = nullptr;
#endif

    // RComponentArea::print(areaArray, ("Array" + std::string("  child").c_str()));

    assert(nullptr == mRootNode);
    bool bArray = true;
    void* pAddr = XYTreeNode::rebalance(&leaf, bArray);
    if (!bArray)
    {
        mRootNode = (XYTreeNode*)pAddr;
        // leaf.removeAreaArray(false);
    }
    else
    {
        // 创建一个子节点(近邻树叶) : 坐标初值MININT, 默认方向垂直分割，且存放树叶
        createTree(-DBL_MAX, XYTREE_SPLIT_X);
        for (auto* area : areaArray)
        {
            addAreaToTree(area);
        }
        // leaf.removeAreaArray(false);
    }
    leaf.removeAreaArray(false);
    // print();
    return mRootNode && !bArray;
}
void RXYTree::print()
{
    if (nullptr == mRootNode)
        return;
    mRootNode->print("", 0);
}
std::vector<ComponentArea*> RXYTree::getCollideAreaArray(double aMinX, double aMinY, double aMaxX, double aMaxY)
{
    assert(mRootNode);
    std::vector<ComponentArea*> resultArray;
    BoundRect2D srcRect(aMinX, aMinY, aMaxX, aMaxY);
    mRootNode->search(srcRect, resultArray);
    return resultArray;
}
bool RXYTree::addAreaToTree(ComponentArea* area)
{
    assert(mRootNode);
    assert(area);

    XYTreeChildType childType = XYTREE_CHILD_NUM;
    XYTreeNode* curNode = mRootNode->expandBoundToLeaf(area->getBoundRect(), childType);
    assert(curNode && curNode->isChildAreaArray(childType));
    curNode->addLeafArea(childType, area);
    return true;
}

}  // namespace Telos