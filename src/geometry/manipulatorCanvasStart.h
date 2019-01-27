#ifndef GEOMETRY_LAYOUTSTART_H
#define GEOMETRY_LAYOUTSTART_H

#include <QObject>

#include "manipulator.h"

namespace geometry {

class CanvasStart
        : public ManipulatorTool
{
public:
    CanvasStart(Manipulator* m);

    static
    PManipulatorTool create(Manipulator* m);

public:
    void do_prepare();
    void do_click(point2d, PNeighbourhood nei);
    QString helperText() const;

};

} // namespace geometry

#endif // GEOMETRY_LAYOUTSTART_H
