#ifndef ENDCUPJOINER_H
#define ENDCUPJOINER_H

#include "joiner.h"

namespace geometry
{

// конец линии
MAKESMART(EndCupJoiner);
class EndCupJoiner
        : public Joiner
{
    EndCupJoiner(Scene *sc, point3d p0, point3d dir);
public:
    static PObject create(Scene *sc, point3d p0, point3d dir);
    static PObject createFromList(Scene* sc, QList<NodeInfo> list);

    virtual void draw(GraphicsScene* gscene, GItems &gitems, int level);
    virtual double distanceToPoint(int) const;
    virtual PObject cloneMove(Scene*scene, point3d delta);
    virtual PObject cloneMoveRotate(Scene *scene, point3d delta, point3d center, double angle);
};

MAKESMART(EndCupJoiner);

}

#endif // ENDCUPJOINER_H
