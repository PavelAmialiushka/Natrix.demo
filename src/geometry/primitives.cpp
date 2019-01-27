#include "primitives.h"
#include "drawer.h"

namespace geometry
{

#if 0
Primitive::Primitive()
    : style(StyleLine)
    , hover(0)
    , selected(0)
{
}


Primitive::~Primitive()
{
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

PPrimitive Primitive::line(point2d s, point2d e, PrimitiveStyle st)
{
    return PPrimitive(new PrimitiveLine(s,e,st));
}

PrimitiveLine::PrimitiveLine(point2d start, point2d end, PrimitiveStyle st)
    : start(start)
    , end(end)
{
    style = st;
}

void PrimitiveLine::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveLine::clone() const
{
    auto p = line(start, end, style);
    p->hover = hover;
    p->selected = selected;
    return p;
}

///////////////////////////////////////////////////////////////////

PPrimitive Primitive::box(point2d s, PrimitiveStyle st)
{
    return PPrimitive(new PrimitiveBox(s, st));
}

PrimitiveBox::PrimitiveBox(point2d start, PrimitiveStyle st)
    : start(start)
{
    style = st;
}

void PrimitiveBox::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveBox::clone() const
{
    auto p = box(start, style);
    p->hover = hover;
    p->selected = selected;
    return p;
}

///////////////////////////////////////////////////////////////////

PPrimitive Primitive::spline(QList<point2d> points, PrimitiveStyle st)
{
    return PPrimitive(new PrimitiveSpline(points, st));
}

PrimitiveSpline::PrimitiveSpline(QList<point2d> points, PrimitiveStyle st)
    : points(points)
{
    style = st;
}

void PrimitiveSpline::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveSpline::clone() const
{
    auto p = spline(points, style);
    p->hover = hover;
    p->selected = selected;
    return p;
}

////////////////////////////////////////////////////////////////////////

PPrimitive Primitive::circle(point2d point, double rad, PrimitiveStyle st)
{
    return PPrimitive(new PrimitiveCircle(point, rad, st));
}

PrimitiveCircle::PrimitiveCircle(point2d c, double r, PrimitiveStyle st)
    : center(c)
    , radius(r)
{
    style = st;
}

void PrimitiveCircle::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveCircle::clone() const
{
    auto p = circle(center, radius, style);
    p->hover = hover;
    p->selected = selected;
    return p;
}

////////////////////////////////////////////////////////////////////////

PPrimitive Primitive::spaceOn(QList<point2d> points)
{
    return PPrimitive(new PrimitiveSpaceOn(points));
}

PrimitiveSpaceOn::PrimitiveSpaceOn(QList<point2d> const& points)
    : points(points)
{
}

void PrimitiveSpaceOn::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveSpaceOn::clone() const
{
    auto p = spaceOn(points);
    p->hover = hover;
    p->selected = selected;
    return p;
}

/////////////////////////////////////////////////////////////////////////

PPrimitive Primitive::spaceOff()
{
    return PPrimitive(new PrimitiveSpaceOff());
}

PrimitiveSpaceOff::PrimitiveSpaceOff()
{
}

void PrimitiveSpaceOff::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveSpaceOff::clone() const
{
    auto p = spaceOff();
    p->hover = hover;
    p->selected = selected;
    return p;
}

///////////////////////////////////////////////////////////////////////////

PPrimitive Primitive::text(TextInfo textInfo)
{
    return PPrimitive(new PrimitiveText(textInfo));
}

PrimitiveText::PrimitiveText(TextInfo textInfo)
    : info(textInfo)
{
}

void PrimitiveText::drawMe(Drawer* drawer)
{
    drawer->draw(*this);
}

PPrimitive PrimitiveText::clone() const
{
    auto p = text(info);
    p->hover = hover;
    p->selected = selected;
    return p;
}

#endif
}
