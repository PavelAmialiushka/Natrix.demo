#define _USE_MATH_DEFINES

#include "endcupjoiner.h"
#include "scene.h"
#include "global.h"
#include "objectfactory.h"

#include "graphics.h"
#include "graphicsScene.h"

namespace geometry
{

CREATE_CLASS_BY_NODELIST(EndCupJoiner);

EndCupJoiner::EndCupJoiner(Scene *sc, point3d p0, point3d dir)
    : Joiner(sc, 1)
{
    nodeAt(0)->setPoint(sc->worldSystem(), p0, dir);
}

PObject EndCupJoiner::create(Scene *sc, point3d p0, point3d dir)
{
    PObject object(new EndCupJoiner(sc, p0, dir));
    object->setPObject(object);
    return object;
}

PObject EndCupJoiner::createFromList(Scene* sc, QList<NodeInfo> list)
{
    Q_ASSERT( list.size() == 1);
    return create(sc, list[0].globalPoint, list[0].direction);
}

PObject EndCupJoiner::cloneMove(Scene *scene, point3d delta)
{
    auto result = create(scene, globalPoint(0)+delta, direction(0));
    result->applyStyle(pObject());
    return result;
}

PObject EndCupJoiner::cloneMoveRotate(Scene *scene, point3d delta, point3d center, double angle)
{
    auto a = delta + globalPoint(0).rotate3dAround(center, point3d::nz, angle);
    auto da = direction(0).rotate3dAround(0, point3d::nz, angle);

    auto result = create(scene, a, da);
    result->applyStyle(pObject());
    return result;
}

double EndCupJoiner::distanceToPoint(int) const
{
    return 0.1;
}

void EndCupJoiner::draw(GraphicsScene* gscene, GItems &g, int level)
{
    QPen linePen(QBrush(Qt::black), 1.5, Qt::SolidLine, Qt::RoundCap);

    point3d pp = nodeAt(0)->globalPoint();
    point3d dd = nodeAt(0)->direction();

    point2d p = nodeAt(0)->localPoint();
    point2d d = gscene->ws()->toUser(pp + endCupStrokeHalfSize * dd);

    point2d a = d.rotate2dAround(p, M_PI_2);
    point2d b = d.rotate2dAround(p, -M_PI_2);

    point2d dt = gscene->ws()->toUser(pp + endCupStrokeHalfSize * dd / 2);
    point2d m = dt.rotate2dAround(p, M_PI_2 * 60 / 90);
    point2d n = dt.rotate2dAround(p, -M_PI_2 * 120 / 90);

    QList<point2d> points;
    points << a << m << p << n << b;

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.moveTo(points[0].x, points[0].y);
    for(int index=1; index < points.size(); ++index)
    {
        path.lineTo(points[index].x, points[index].y);
    }

    GItemsList items(gscene);
    items << GraphicsClipItem::create(gscene, path, linePen);

    bool dashDot = lineStyle_ == LineDashDotStyle;
    if (!dashDot)
        g.items << items;

    if (!level) return;

    // рисуем контур
    points = makePointListFromItemsList(items);

    enlargeRegion_hull(points, 2);
    g.contur << drawSpline(gscene, points);
}

}
