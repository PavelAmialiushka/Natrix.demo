#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include "label.h"
#include "marker.h"

#include <QString>
#include <QRectF>

class QDomElement;

namespace geometry
{

class Scene;
class GraphicsTextItem;

class TextLabel;

auto isText = [&](PLabel a) { return a.dynamicCast<TextLabel>(); };
auto toText = [&](PLabel a) { return a.dynamicCast<TextLabel>(); };

///////////////////////////////////////////////////////////////////

class TextLabel
        : public Label
{
    TextInfo info_;
    QWeakPointer<GraphicsTextItem> item_;    
    double height_, width_;

public:
    TextLabel(Scene*, TextInfo info);
    ~TextLabel();
    static PLabel create(Scene*, point3d point, TextInfo info);
    static PLabel create(Scene*, TextInfo info);

    // serialization
    static PLabel createFrom(Scene*, class QDomElement);
    virtual void saveLabel(QDomElement);
    virtual bool loadLabel(QDomElement);

    virtual void draw(GraphicsScene* gscene, GItems &gitems, int level);
    virtual PLabel clone(Scene*, point2d delta) const;
public:

    void updateText(QString);
    TextInfo info() const;
    void setInfo(TextInfo);

    // выполняется во время редактирования
    void selectAll();

    QSharedPointer<GraphicsTextItem> item() const;
private:
    QList<point2d> getCornerPoints();
    void setActualSize(double w, double h);
    void updateStickyMode(QList<PMarker> markers, struct TextLayout& self);
public:
    virtual double distance(const point2d &pt);
};

typedef QSharedPointer<TextLabel> PTextLabel;

}
#endif // TEXTLABEL_H
