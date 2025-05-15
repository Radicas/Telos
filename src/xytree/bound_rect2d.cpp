#include "Telos/xytree/bound_rect2d.h"

#include <assert.h>

namespace Telos
{

BoundRect2D::BoundRect2D() : mMinX(DBL_MAX), mMinY(DBL_MAX), mMaxX(-DBL_MAX), mMaxY(-DBL_MAX) {}

BoundRect2D::BoundRect2D(double aMinX, double aMinY, double aMaxX, double aMaxY)
    : mMinX(aMinX), mMinY(aMinY), mMaxX(aMaxX), mMaxY(aMaxY)
{
    assert(aMinX <= aMaxX && aMinY <= aMaxY);
}

void BoundRect2D::setBound(double aMinX, double aMinY, double aMaxX, double aMaxY)
{
    assert(aMinX <= aMaxX && aMinY <= aMaxY);
    mMinX = aMinX;
    mMinY = aMinY;
    mMaxX = aMaxX;
    mMaxY = aMaxY;
}
void BoundRect2D::setBound(const BoundRect2D* aSrcBound)
{
    assert(aSrcBound && aSrcBound->isValid());
    setBound(aSrcBound->mMinX, aSrcBound->mMinY, aSrcBound->mMaxX, aSrcBound->mMaxY);
}
void BoundRect2D::expandBound(const BoundRect2D* aSrcBound)
{
    assert(aSrcBound && aSrcBound->isValid());
    if (mMinX > aSrcBound->mMinX)
        mMinX = aSrcBound->mMinX;
    if (mMinY > aSrcBound->mMinY)
        mMinY = aSrcBound->mMinY;
    if (mMaxX < aSrcBound->mMaxX)
        mMaxX = aSrcBound->mMaxX;
    if (mMaxY < aSrcBound->mMaxY)
        mMaxY = aSrcBound->mMaxY;

    assert(isValid());
}

bool BoundRect2D::isDisjoint(const BoundRect2D* aSrcBound) const
{
    assert(aSrcBound && aSrcBound->isValid());
    return aSrcBound->mMinX > this->mMaxX || aSrcBound->mMaxX < this->mMinX || aSrcBound->mMinY > this->mMaxY ||
           aSrcBound->mMaxY < this->mMinY;
}
bool BoundRect2D::isContains(const BoundRect2D* aSrcBound) const
{
    assert(aSrcBound && aSrcBound->isValid());
    return aSrcBound->mMinX >= this->mMinX && aSrcBound->mMaxX <= this->mMaxX && aSrcBound->mMinY >= this->mMinY &&
           aSrcBound->mMaxY <= this->mMaxY;
}
bool BoundRect2D::isEqual(const BoundRect2D* aSrcBound) const
{
    assert(aSrcBound && aSrcBound->isValid());
    return aSrcBound->mMinX == this->mMinX && aSrcBound->mMaxX == this->mMaxX && aSrcBound->mMinY == this->mMinY &&
           aSrcBound->mMaxY == this->mMaxY;
}
}  // namespace Telos