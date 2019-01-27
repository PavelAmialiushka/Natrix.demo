#ifndef GHOSTADORNER_H
#define GHOSTADORNER_H

#include "adorner.h"
#include "object.h"

namespace geometry
{

struct GItemsList;

class GhostAdorner
        : public Adorner
{
    QWeakPointer<ObjectToSelect> sample;
    QScopedPointer<GItemsList> cache;
public:
    GhostAdorner(PObjectToSelect object);
    virtual void draw(GraphicsScene* gscene, GItems &gitems, int level) override;

private:
    void createCache(GraphicsScene* gscene, GItems &g);
};

}
#endif // GHOSTADORNER_H
