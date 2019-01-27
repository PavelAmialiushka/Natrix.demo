#ifndef CANVASRECTANGLE_H
#define CANVASRECTANGLE_H

#include "label.h"

namespace geometry
{

MAKESMART(CanvasRectangle);

struct CanvasInfo
{
    point2d leftTop;
    point2d rightBottom;
    point2d center;
    int     index;

    static CanvasInfo def();
};

class CanvasRectangle
        : public Label
{
    CanvasInfo  info_;

    CanvasRectangle(Scene*, CanvasInfo c);
public:
    static PLabel create(Scene* scene, CanvasInfo info);
    static PLabel createFrom(Scene*, class QDomElement);

    CanvasInfo info() const;
    void setInfo(CanvasInfo);

    virtual void saveLabel(QDomElement elm);

    virtual void draw(GraphicsScene* gscene, GItems &gitems, int level);
    virtual double distance(point2d const& pt);
    virtual PLabel clone(Scene*, point2d delta) const;
    virtual QBrush originalColour() const;

    QRectF makeQRect() const;
};

auto toCanvasRectangle=[](PLabel l) { return l.dynamicCast<CanvasRectangle>(); };

auto byCanvasIndex = [](PCanvasRectangle lhs, PCanvasRectangle rhs)
{
    return lhs->info().index < rhs->info().index;
};

}
#endif // CANVASRECTANGLE_H
