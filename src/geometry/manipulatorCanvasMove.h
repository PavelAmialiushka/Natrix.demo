#ifndef GEOMETRY_LAYOUTMOVE_H
#define GEOMETRY_LAYOUTMOVE_H

#include <QObject>
#include "manipulator.h"

namespace geometry {

class CanvasMove
        : public ManipulatorTool
{
    point2d start_;
    PCanvasRectangle  canvas_;
public:
    CanvasMove(Manipulator* m, point2d pt, PCanvasRectangle label);

    static
    PManipulatorTool create(Manipulator* m, point2d pt, PCanvasRectangle label);

public:
    void do_prepare();
    void do_click(point2d pt, PNeighbourhood n);
    void do_drag(point2d pt, PNeighbourhood, bool started = false);
    void do_drop(point2d pt, PNeighbourhood n);

    QString helperText() const;
};

} // namespace geometry

#endif // GEOMETRY_LAYOUTMOVE_H
