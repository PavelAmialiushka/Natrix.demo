#include "scenePropertyValue.h"

namespace geometry
{


bool ScenePropertyValue::empty() const
{
    return current==-1;
}

void ScenePropertyValue::reset(ScenePropertyType t)
{
    type = t;

    active.clear();
    exclusive = true;
    current = -1;
}

void ScenePropertyValue::addValue(int index)
{
    while (index >= active.size())
        active << false;

    if (current == -1)
        current = index;
    else if (current != index)
        exclusive = false;

    active[index] = true;
}

}
