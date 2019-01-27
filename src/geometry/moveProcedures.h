#ifndef MOVEINITDATA_H
#define MOVEINITDATA_H

#include "object.h"
#include "command.h"
#include "adorner.h"
#include "marker.h"

#include <QMap>

namespace geometry
{

struct MoveData
{
    // конечная точка движения
    point3d destination;
    // начальная точка движения
    point3d startPoint;
    // объект, от которого начинается движение
    PObject object;

    // объекты, которые по условию двигать нельзя
    QList<PObject> fixedObjects;

    // при сжатии линий происходит замена объектов
    // здесь хранится информация о исходных и новых объектах
    // autoReplace[original] = newbyObject;
    // markerReplaced[originalMarker] = newbyMarker;
    QMap<PObject, PObject> autoReplaced;
    QMap<PMarker, PMarker> markerReplaced;

    // возвращает самую свежую версию маркера
    PMarker replacedMarker(PMarker m) const;

    QMap<PObjectToSelect, PAdorner> adornerCache;

    template<class T>
    T replaced(T original) const
    {
        PObject obj = replacedPObject(original);
        T result = qSharedPointerDynamicCast<typename T::value_type>(obj);
        Q_ASSERT(result);
        return result;
    }

    template<class T>
    T originalOf(T original) const
    {
        PObject obj = originalOfPObject(original);
        T result = qSharedPointerDynamicCast<typename T::value_type>(obj);
        Q_ASSERT(result);
        return result;
    }

    PNode originalOf(PNode rNode) const;

    // поиск нового объекта на основе исходного
    PObject replacedPObject(PObject original) const;
    PObject originalOfPObject(PObject next) const;
};

struct NodeMovement
{
    point3d delta;
};

enum class ObjectRestrictionType
{
    Unknown, // не установлено
    Elastic, // линия стремится поглотить движение
    Rigid, // линия стремится сохранить размер, (но может менять)
    Solid,  // не изменяется вообще (это не линия)
    Fixed, // второй конец линии двигать категорически нельзя
    Resize, // изменение размера жестко определено по условиям
};

struct ObjectRestriction
{
    point3d point;
    ObjectRestrictionType rule;
};

struct MoveParameters
{
    ObjectRestrictionType defaultObjectRestriction;

    QMap<PNode, NodeMovement> nodeMoves;
    QMap<PObject, ObjectRestriction> objectRules;
};

bool isSameContur(Scene* scene, PObject start, PObject end);
bool isSameContur(Scene* scene, PObject start, PObject end, MoveParameters& param);

Command cmdShiftObject(Scene* scene, MoveData& data, bool* success=0);
Command cmdMoveObject(Scene* scene, MoveData& data, bool* success=0);
Command cmdMoveConnectObject(Scene* scene, MoveData& data, PObject landObject, bool* success=0);

Command cmdMoveNodes(Scene* scene, MoveData& data, MoveParameters& params, bool* success=0);

}

#endif // MOVEINITDATA_H
