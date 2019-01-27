#include "BaseObject.h"

#include "graphics.h"

namespace geometry
{

ObjectToDraw::~ObjectToDraw()
{
}

QBrush ObjectToDraw::originalColour() const
{
    return graphics::originalColour();
}

}
