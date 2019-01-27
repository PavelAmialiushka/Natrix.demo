#ifndef NEIGHBOURHOOD_H
#define NEIGHBOURHOOD_H

#include "scene.h"
#include "object.h"
#include "label.h"
#include "grip.h"

namespace geometry
{

struct ObjectInfo
{
    double distance; // расстояние на чертеже
    bool isNodeFound; // найдена нода (а не другая часть объекта)
    int nodeNo;   // если нода ближе всего, то номер ноды
    point3d closestPoint;

    PNode node(PObject) const;
};

struct ConnectionData
{
    point3d    point;
    ObjectInfo info;
    PObject    object;

    PNode node() const;
};

//////////////////////////////////////////////////////

struct LabelInfo
{
    double distance;
};

struct GripInfo
{
    double distance;
};

//////////////////////////////////////////////////////

struct Neighbourhood
{
    QMap<PObject, ObjectInfo> objectInfo;
    QList<PObject> closestObjects;

    QList<PLabel> closestLabels;
    QMap<PLabel, LabelInfo> labelInfo;

    QList<PGrip> closestGrips;
    QMap<PGrip, GripInfo> gripInfo;

    QMap<PObjectToSelect, HoverState> hoverState;
};

}

#endif // NEIGHBOURHOOD_H
