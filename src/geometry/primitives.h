#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "point.h"
#include "textInfo.h"

#include <QString>
#include <QSharedPointer>

namespace geometry
{

//////////////////////////////////////////////////////////////////

class Primitive;
typedef QSharedPointer<Primitive> PPrimitive;

//////////////////////////////////////////////////////////////////

enum PrimitiveStyle {
    NoStyle,
    StyleLine,   // линия
    StyleText,   // текст
    StyleElement,  // задвижки и проч
    StyleDashDot, // штрихпунктир
    StyleBlackFilled, // заливка черным цветом
    StyleViewport, // линии, изображающие границы области рисования

    // adorners
    StyleAdornerCursorLine,  // line of cursor point

    //
    StyleStraightLineAdorner, // вспомогательные линии
    StyleMoveLineAdorner,
    StyleGhostAdorner,

    // nodepointadorners
    StyleNodePointAdorner=100,  // здесь должно хватить номеров, чтобы
                                // вместились все стили NodePointAdorner

    // not to setup
    PrivateStyleHover = 500,
    PrivateStyleNewby,
    PrivateStyleTextHover
} ;

enum PrimitiveAdornerStyle {

};

#if 0
//////////////////////////////////////////////////////////////////
class Drawer;

class Primitive
{
public:
    PrimitiveStyle style;
    int hover;
    int selected;

public:
    Primitive();
    virtual ~Primitive();
    virtual void drawMe(Drawer*)=0;
    virtual PPrimitive clone() const=0;

public:
    static PPrimitive line(point2d s, point2d e, PrimitiveStyle st = StyleLine);
    static PPrimitive box(point2d, PrimitiveStyle st);
    static PPrimitive circle(point2d, double, PrimitiveStyle st);
    static PPrimitive spline(QList<point2d> points, PrimitiveStyle st = StyleStraightLineAdorner);
    static PPrimitive spaceOn(QList<point2d> points);
    static PPrimitive text(TextInfo textInfo);
    static PPrimitive spaceOff();
};

//////////////////////////////////////////////////////////////////

class PrimitiveLine
        : public Primitive
{
public:
    point2d start;
    point2d end;

    PrimitiveLine(point2d start, point2d end, PrimitiveStyle st = StyleLine);
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

//////////////////////////////////////////////////////////////////

class PrimitiveBox
        : public Primitive
{
public:
    point2d            start;

    PrimitiveBox(point2d start, PrimitiveStyle style);
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

class PrimitiveSpline
        : public Primitive
{
public:
    QList<point2d> points;

    PrimitiveSpline(QList<point2d> points, PrimitiveStyle st);
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

class PrimitiveCircle
        : public Primitive
{
public:
    point2d center;
    double  radius;

    PrimitiveCircle(point2d c, double r, PrimitiveStyle st);
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

class PrimitiveSpaceOn
        : public Primitive
{
public:
    QList<point2d> points;

    PrimitiveSpaceOn(QList<point2d> const& points);
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

class PrimitiveSpaceOff
        : public Primitive
{
public:
    PrimitiveSpaceOff();
    virtual void drawMe(Drawer*);
    virtual PPrimitive clone() const;
};

class PrimitiveText
        : public Primitive
{
public:
    TextInfo info;

public:
    PrimitiveText(TextInfo p);
    virtual void drawMe(Drawer *);
    virtual PPrimitive clone() const;
};

#endif
}

#endif // PRIMITIVE_H
