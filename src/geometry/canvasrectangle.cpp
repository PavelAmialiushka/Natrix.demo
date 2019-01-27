#include "canvasrectangle.h"

#include "graphics.h"
#include "graphicsScene.h"

namespace geometry
{

CREATE_LABEL_BY_DOMELEMENT(CanvasRectangle);

PLabel CanvasRectangle::createFrom(Scene* scene, class QDomElement elm)
{
    bool ok;
    point2d pt = point2d::serialLoad(elm.attribute("center"), &ok);
    if (!ok) pt = 0;

    int index = elm.attribute("index").toInt();

    CanvasInfo inf;
    inf.index = index;
    inf.center = pt;
    point2d halfSize = point2d(1188, 840)/2;
    inf.leftTop = inf.center - halfSize;
    inf.rightBottom = inf.center + halfSize;

    return CanvasRectangle::create(scene, inf);}

CanvasInfo CanvasInfo::def()
{
    CanvasInfo info;
    info.index = -1;
    info.leftTop = 0;
    info.rightBottom = point2d(1188, 840);
    info.center = info.rightBottom / 2;
    return info;
}

void CanvasRectangle::saveLabel(QDomElement elm)
{
    elm.setAttribute("center", info_.center.serialSave());
    elm.setAttribute("index", info_.index);
}

CanvasRectangle::CanvasRectangle(Scene* scene, CanvasInfo inf)
    : Label(scene)
    , info_(inf)
{
    point2d s = inf.rightBottom - inf.leftTop;
    point2d c = inf.leftTop + s / 2;

    setSize(s.x, s.y);
    moveCenterTo(c);
}

PLabel CanvasRectangle::create(Scene* scene, CanvasInfo info)
{
    return PLabel( new CanvasRectangle(scene, info) );
}

CanvasInfo CanvasRectangle::info() const
{
    return info_;
}

void CanvasRectangle::setInfo(CanvasInfo info)
{
    info_ = info;
}

QBrush CanvasRectangle::originalColour() const
{
    return graphics::viewportPen().brush();
}

QRectF CanvasRectangle::makeQRect() const
{
    return QRectF(leftTop_.x, leftTop_.y,
                  rightBottom_.x - leftTop_.x,
                  rightBottom_.y - leftTop_.y);
}

void CanvasRectangle::draw(GraphicsScene* gscene, GItems &g, int level)
{
    g.items << gscene->addLine(makeQLine(leftTop(), leftBottom()), graphics::viewportPen());
    g.items << gscene->addLine(makeQLine(leftBottom(), rightBottom()), graphics::viewportPen());
    g.items << gscene->addLine(makeQLine(rightBottom(), rightTop()), graphics::viewportPen());
    g.items << gscene->addLine(makeQLine(rightTop(), leftTop()), graphics::viewportPen());

    if (!level) return;

    // выделяем только сами прямые, исключаем внутреннюю область
    QList<point2d> cnt;
    cnt << leftTop() << leftBottom() << rightBottom() << rightTop () << leftTop()
        << leftTop() << rightTop() << rightBottom() << leftBottom() << leftTop();
    enlargeRegion_oneSide(cnt, 6);

    g.contur << drawSpline(gscene, cnt, Qt::NoPen);
}

double CanvasRectangle::distance(point2d const& pt)
{
    double d1 = pt.distance_to_line(leftTop(), leftBottom());
    double d2 = pt.distance_to_line(leftBottom(), rightBottom());
    double d3 = pt.distance_to_line(rightBottom(), rightTop());
    double d4 = pt.distance_to_line(rightTop(), leftTop());
    return std::min(
                std::min(d1, d2), std::min(d3,d4));
}

PLabel CanvasRectangle::clone(Scene* scene, point2d delta) const
{
    CanvasInfo inf = info();
    inf.center += delta;
    inf.leftTop += delta;
    inf.rightBottom += delta;

    auto canvas = new CanvasRectangle(scene, inf);
    return PLabel( canvas );
}

}
