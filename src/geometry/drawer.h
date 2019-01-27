#ifndef DRAWER_H
#define DRAWER_H

#include "primitives.h"

namespace geometry
{

#if 0
class Scene;

class Drawer
{
public:
    Drawer();

    void redraw(Scene const*, double scale, bool exporting = false);

private:
    void draw(PPrimitive);
    void draw_cross(Scene const* scene);
    virtual void changeScale(double)=0;
public:
    virtual void draw(PrimitiveLine const&)=0;
    virtual void draw(PrimitiveCircle const&)=0;
    virtual void draw(PrimitiveBox const&)=0;
    virtual void draw(PrimitiveSpline const&)=0;
    virtual void draw(PrimitiveSpaceOn const&)=0;
    virtual void draw(PrimitiveSpaceOff const&)=0;
    virtual void draw(PrimitiveText const&)=0;
};

#endif
}

#endif // DRAWER_H
