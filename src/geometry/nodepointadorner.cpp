#include "nodepointadorner.h"

#include "scene.h"

#include "graphics.h"
#include "graphicsScene.h"

namespace geometry
{

NodePointAdorner::NodePointAdorner(point3d point, NodePointAdornerStyle style)
    : point_(point)
    , style_(style)
{
}


void NodePointAdorner::draw(GraphicsScene* gscene, GItems &g, int /*level*/)
{
    WorldSystem& ws = *gscene->ws();
    auto point = ws.toUser(point_);

    QPen pen;
    switch(style_)
    {
    case AdornerLineFromFree: // черный
        pen = QPen(QBrush(QColor(0,0,0,128)), 3, Qt::SolidLine, Qt::RoundCap);
        break;

    case AdornerLineFromNode:
    case AdornerMovingToNode:
    case AdornerMovingFromNode: // синий
        pen = QPen(QBrush(QColor(0,0,255,128)), 3, Qt::SolidLine, Qt::RoundCap);
        break;

    case AdornerMovingToFree:
    case AdornerLineFromNone: // красный
        pen = QPen(QBrush(QColor(255,0,0,128)), 3, Qt::SolidLine, Qt::RoundCap);
        break;

    case AdornerLineFromLine:
    case AdornerMovingToLine:
    case AdornerMovingFromLine: // зеленоватый
        pen = QPen(QBrush(QColor(21,179,0,128)), 3, Qt::SolidLine, Qt::RoundCap);
        break;

    case AdornerOverWeld:
    case AdornerMovingWeld: // оранжевый
        pen = QPen(QBrush(QColor(255,90,255,128)), 3, Qt::SolidLine, Qt::RoundCap);
        break;
    }

    if (style_ < AdornerMovingFromNode)
    {
        // line styles
        g.items << drawBox(gscene, point, 4, pen);
    }
    else if (style_ < AdornerOverWeld){
        // moving styles
        g.items << drawCircle(gscene, point, 4, pen);
    } else
    {
        g.items << drawDiamond(gscene, point, 4, pen);
    }
}

bool NodePointAdorner::isTemporal() const
{
    return true;
}
}

