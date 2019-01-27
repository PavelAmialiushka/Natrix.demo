#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "point.h"
#include "makeSmart.h"
#include "primitives.h"

class QBrush;
class QGraphicsItem;
class QGraphicsPathItem;

namespace geometry
{

class Scene;
class GraphicsScene;

MAKESMART(ObjectToDraw);
MAKESMART(ObjectToSelect);

typedef QSharedPointer<QGraphicsItem> PGItem;

struct GItemsList;
struct GItems;

///////////////////////////////////////////////////////////////////////////////

class ObjectToDraw
        : public QEnableSharedFromThis<ObjectToDraw>
{
public:
    virtual ~ObjectToDraw()=0;

    virtual void draw(GraphicsScene* gscene, GItems &gitems, int level)=0;
    virtual double distance(point2d const& pt)=0;
    virtual QBrush originalColour() const;
};

///////////////////////////////////////////////////////////////////////////////

class ObjectToSelect
        : public ObjectToDraw
{
};

///////////////////////////////////////////////////////////////////////////////


}

#endif // BASEOBJECT_H
