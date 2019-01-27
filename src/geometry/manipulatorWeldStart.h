#ifndef MANIPULATORSTARTWELD_H
#define MANIPULATORSTARTWELD_H

#include "manipulatorTools.h"

namespace geometry
{

class WeldStart
        : public ManipulatorTool
{
    Q_OBJECT
public:
    WeldStart(Manipulator* m);
    static PManipulatorTool create(Manipulator* m);

public:
    void do_click(point2d, PNeighbourhood);
    QString helperText() const;

};

}

#endif // MANIPULATORSTARTWELD_H
