#ifndef MANIPULATORMOVE_H
#define MANIPULATORMOVE_H

#include "manipulatorTools.h"


namespace geometry
{

class MoveStart
        : public ManipulatorTool
{
    Q_OBJECT

public:
    MoveStart(Manipulator* m);

    static
    PManipulatorTool create(Manipulator* m);

public:
    void do_click(point2d, PNeighbourhood);
    QString helperText() const;
};

}

#endif // MANIPULATORMOVE_H
