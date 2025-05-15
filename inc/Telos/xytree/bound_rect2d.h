#ifndef BOUND_RECT2D_H
#define BOUND_RECT2D_H

#include <cfloat>
#include <vector>

namespace Telos
{
class BoundRect2D
{
   private:
    double mMinX;
    double mMinY;
    double mMaxX;
    double mMaxY;

   public:
    BoundRect2D();
    BoundRect2D(double aMinX, double aMinY, double aMaxX, double aMaxY);

    void setBound(double aMinX, double aMinY, double aMaxX, double aMaxY);
    void setBound(const BoundRect2D* aSrcBound);
    void expandBound(const BoundRect2D* aSrcBound);

    bool isDisjoint(const BoundRect2D* aSrcBound) const;
    bool isContains(const BoundRect2D* aSrcBound) const;
    bool isEqual(const BoundRect2D* aSrcBound) const;
    bool isValid() const { return mMinX <= mMaxX && mMinY <= mMaxY; }

    double getMinX() const { return mMinX; }
    double getMinY() const { return mMinY; }
    double getMaxX() const { return mMaxX; }
    double getMaxY() const { return mMaxY; }
};
}  // namespace Telos

#endif  // BOUND_RECT2D_H