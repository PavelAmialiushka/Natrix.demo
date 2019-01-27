#ifndef SCENEPROPERTYVALUE_H
#define SCENEPROPERTYVALUE_H

#include <QList>

namespace geometry
{

class SceneProperties;

enum class ScenePropertyType
{
    TextSize, TextRotation, TextDecorection,
    ValveSize, ValveFlanges,
    LineStyle, BendStyle, TeeStyle,
};

struct ScenePropertyValue
{
    ScenePropertyType type;
    QList<bool> active;
    bool     exclusive; // выбран один активный
    int      current;   // активный

    bool empty() const;
    void reset(ScenePropertyType t);
    void addValue(int index);
};

}

#endif // SCENEPROPERTYVALUE_H
