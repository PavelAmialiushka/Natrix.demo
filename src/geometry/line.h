#ifndef LINE_H
#define LINE_H

#include "object.h"
#include "lineinfo.h"

#include <QVector>

namespace geometry
{

//////////////////////////////////
//
// класс линии, может присоединяться только к джойнерам и к элементам
// может растягиваться

MAKESMART(Line);
class Line
    : public Object
{
    int   lineStyle_;
    Line(Scene *sc, point3d p1, point3d p2);
public:
    static PObject create(Scene *sc, point3d p1, point3d p2, PObject sample);
    static PObject createFromList(Scene* sc, QList<NodeInfo> list);
    virtual double width() const;

    virtual void applyStyle(PObject);
    virtual PObject cloneMove(Scene*, point3d delta);
    virtual PObject cloneMoveRotate(Scene *scene, point3d delta, point3d center, double angle);

    int lineStyle() const;
    void setLineStyle(int s);
    bool apply(ScenePropertyValue v);
    virtual void applyStyle(LineInfo);

private:
    virtual void draw(GraphicsScene*, GItems &gitems, int level);

    virtual void saveObject(QVariantMap& map);
    virtual bool loadObject(QVariantMap map);
};

}
#endif // LINE_H
